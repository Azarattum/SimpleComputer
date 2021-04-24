#ifndef MACHINE_GUARD
#define MACHINE_GUARD

#include "coder.cpp"
#include "cpu.cpp"
#include "iodevice.cpp"
#include "memory.cpp"
#include "timer.cpp"

class Machine {
public:
  CPU *cpu;
  Memory *mem;
  Timer *timer;
  IODevice *io;

  bool running = false;
  std::function<void()> updated_callback;

  Machine() {
    auto callback = [this]() { this->tick(); };

    cpu = new CPU();
    mem = new Memory();
    io = new IODevice();
    timer = new Timer(callback, 150);

    reset();
  }

  void tick() {
    if (!cpu->tick(mem, io)) {
      timer->stop();
      running = false;
    }
    updated_callback();
  }

  void reset() {
    cpu->reset();
    mem->reset();
  }

  void run() {
    running = true;
    timer->start();
  }

  void load_program(const char *filename) { mem->load(filename); }

  void interrupt() {
    timer->stop();
    cpu->reset();
    running = false;
  }

  void skip_forward() {
    if (cpu->pc < (mem->size - 1)) {
      cpu->pc++;
    }
  }

  void skip_back() {
    if (cpu->pc > 0) {
      cpu->pc--;
    }
  }

  void overwrite_opcode(Opcode opcode) {
    Command command = Coder::decode((*mem)[cpu->pc]);
    command.valid = true;
    command.opcode = opcode;
    (*mem)[cpu->pc] = Coder::encode(command);
  }

  void overwrite_operand(Word operand) {
    Command command = Coder::decode((*mem)[cpu->pc]);
    command.valid = true;
    command.operand = operand;
    (*mem)[cpu->pc] = Coder::encode(command);
  }

  void overwrite_accumulator(Word value) { cpu->ac = value; }

  ~Machine() {
    delete cpu;
    delete mem;
  }
};

#endif