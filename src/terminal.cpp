#include "graphics/big.view.cpp"
#include "graphics/cpu.view.cpp"
#include "graphics/help.view.cpp"
#include "graphics/io.view.cpp"
#include "graphics/mem.view.cpp"
#include "machine.cpp"
#include <functional>
#include <readline/readline.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

using Size = struct winsize;

void *self;

class Terminal {
private:
  bool isRunning = false;
  bool isRendered = false;
  bool isInteractive = false;
  Machine *machine;
  std::vector<Frame *> views;
  Ioview *mainIO;

  std::vector<std::string> history;
  short historySize;

  Size getSize() {
    Size size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    return size;
  }

  bool handleCommand(std::string input) {
    if (!input.find("next") || input == "n")
      next(0, 0);
    if (!input.find("prev") || input == "p")
      prev(0, 0);
    if (!input.find("reset") || input == "r")
      machine->reset();
    if (!input.find("execute") || input == "e") {
      mainIO->writeLine("\rRunning... (^C to interrupt)");
      render();
      isRunning = true;
      machine->run();
    }
    if (!input.find("step") || input == "t")
      machine->tick();
    if (!input.find("quit") || !input.find("exit") || input == "q")
      return false;
    if (!input.find("kill") || input == "k")
      machine->interrupt();
    if (!input.find("data") || !input.find("d ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      Word value = strtol(input.c_str(), NULL, 16);
      (*machine->mem)[machine->cpu->pc] = value;
    }
    if (!input.find("opcode") || !input.find("c ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      Word code = strtol(input.c_str(), NULL, 16);
      machine->overwrite_opcode((Opcode)code);
    }
    if (!input.find("operand") || !input.find("o ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      Word operand = strtol(input.c_str(), NULL, 16);
      machine->overwrite_operand(operand);
    }
    if (!input.find("accumulator") || !input.find("a ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      Word value = strtol(input.c_str(), NULL, 16);
      machine->overwrite_accumulator(value);
    }
    if (!input.find("load") || !input.find("l ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      machine->load_program(input.c_str());
    }
    if (!input.find("save") || !input.find("s ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      machine->mem->dump(input.c_str());
    }
    if (!input.find("counter") || !input.find("i ")) {
      input.erase(input.begin(), input.begin() + input.find(" ") + 1);
      Word value = strtol(input.c_str(), NULL, 16);
      if (value >= 0 && value < machine->mem->size) {
        machine->cpu->pc = value;
      }
    }

    return true;
  }

  static void handleSignal(int signum) {
    if (!self)
      return;
    auto term = ((Terminal *)self);
    if (signum == SIGUSR1) {
      term->machine->interrupt();
    }
    if (signum == SIGINT) {
      if (term->machine->running) {
        term->machine->interrupt();
      } else {
        exit(0);
      }
    }
  }

  static int next(int count, int key) {
    if (!self)
      return 0;
    auto term = ((Terminal *)self);
    term->machine->skip_forward();
    term->render();
    return 0;
  }

  static int kill(int count, int key) {
    kill(getpid(), SIGUSR1);
    return 0;
  }

  static int prev(int count, int key) {
    if (!self)
      return 0;
    auto term = ((Terminal *)self);
    term->machine->skip_back();
    term->render();
    return 0;
  }

  void write(char *text) {
    mainIO->writeLine(text);
    render();
  }

  char *read() {
    char tmp[strlen(rl_line_buffer)];
    strcpy(tmp, rl_line_buffer);

    char *line = mainIO->readLine();
    strcpy(rl_line_buffer, tmp);
    render();

    return line;
  }

public:
  Terminal(Machine *machine) {
    this->machine = machine;
    auto callback = [this]() { this->render(); };
    machine->updated_callback = callback;

    Size size = getSize();
    const int rowHeight = 9;
    const int cpuWidth = 32;
    const int firstSplit = size.ws_col - cpuWidth;
    const int secondSpilt = size.ws_col * 0.33;
    const int thirdSpilt = size.ws_col * 0.66;

    views.push_back(new Memview(0, 0, firstSplit - 1, rowHeight, machine->mem,
                                machine->cpu));
    views.push_back(
        new Cpuview(firstSplit, 0, cpuWidth, rowHeight, machine->cpu));
    views.push_back(new Bigview(0, rowHeight, secondSpilt, rowHeight,
                                machine->cpu, machine->mem));
    views.push_back(new Ioview(secondSpilt, rowHeight, thirdSpilt - secondSpilt,
                               rowHeight));
    mainIO = (Ioview *)(views.back());
    views.push_back(new Helpview(thirdSpilt, rowHeight,
                                 size.ws_col - thirdSpilt, rowHeight));

    historySize = size.ws_row - 20;
  }

  void interactive() {
    self = this;
    isInteractive = true;
    signal(SIGUSR1, handleSignal);
    signal(SIGINT, handleSignal);
    machine->io->written_callback = [this](char *text) { write(text); };
    machine->io->read_callback = [this]() { return read(); };

    char *line;

    rl_bind_keyseq("\e[B", next);
    rl_bind_keyseq("\e[A", prev);
    rl_bind_keyseq("\\C-c", kill);

    for (;;) {
      printf("\r");
      line = readline("> ");
      auto input = std::string(line);
      history.push_back(input);
      if (history.size() > historySize) {
        history.erase(history.begin());
      }

      rl_line_buffer[0] = 0;
      if (!handleCommand(input))
        break;

      render();
    }

    printf("Exiting...\n");
    isInteractive = false;
    self = NULL;
  }

  void render() {
    if (!isRendered) {
      printf("\033[2J");
      isRendered = true;
    }

    if (isRunning && !machine->running) {
      isRunning = false;
      if (machine->cpu->overmemory) {
        mainIO->writeLine("\rFailed! Memory overflow!");
      } else if (machine->cpu->overflow) {
        mainIO->writeLine("\rFailed! Data overflow!");
      } else if (machine->cpu->zerodevion) {
        mainIO->writeLine("\rFailed! Devision by zero!");
      } else if (machine->cpu->wrongcommand) {
        mainIO->writeLine("\rFailed! Wrong command!");
      } else {
        mainIO->writeLine("\rSuccessfully halted.");
      }
    }

    for (auto &&view : views) {
      view->render();
    }

    if (isInteractive) {
      Size size = getSize();
      for (int i = 0; i < history.size(); i++) {
        int count = printf("> %s", history[i].c_str());
        for (int i = 0; i < (size.ws_col - count); i++)
          printf(" ");
        printf("\n");
      }

      if (isRunning)
        return;

      for (int i = 0; i < size.ws_col; i++)
        printf(" ");
      int count = printf("\r> %s", rl_line_buffer);
      fflush(stdout);
    }
  }
};