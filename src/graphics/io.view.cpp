#include "../coder.cpp"
#include "../memory.cpp"
#include "frame.cpp"
#include <readline/readline.h>
#include <vector>

class Ioview : public Frame {
private:
  std::vector<std::string> buffer;

public:
  Ioview(short x, short y, short w, short h) : Frame(x, y, w, h, "I/O") {}

  void render() {
    Frame::render();
    for (auto &&line : buffer) {
      const char *ret = strchr(line.c_str(), '\r');
      printline("%s", ret ? ret + 1 : line.c_str());
    }

    endRender();
  }

  void writeLine(const char *text) {
    buffer.push_back("→ " + std::string(text));
    if (buffer.size() > (h - 2)) {
      buffer.erase(buffer.begin());
    }
  }

  char *readLine() {
    int row = buffer.size();
    if (buffer.size() >= (h - 2)) {
      buffer.erase(buffer.begin());
      row--;
      render();
    }

    switchRow(row);
    char *line = readline("← ");
    buffer.push_back("← " + std::string(line));
    return line;
  }
};