#include "../coder.cpp"
#include "../memory.cpp"
#include "frame.cpp"

class Memview : public Frame {
public:
  Memory *memory;
  CPU *cpu;

  Memview(short x, short y, short w, short h, Memory *memory, CPU *cpu)
      : Frame(x, y, w, h, "Memory") {
    this->memory = memory;
    this->cpu = cpu;
  }

  void render() {
    Frame::render();
    short perline = (w - 3) / 6;
    Byte address = 0;
    for (int i = 0; i < h - 2; i++) {
      printAt(x + 2, y + i + 1);
      for (short j = 0; j < perline; j++) {
        Word value = (*memory)[address];
        if (cpu->pc == address)
          printf("\033[1;7m");

        printf("%s%04X\033[0m ", value < 0 ? "-" : "+",
               value < 0 ? -(unsigned)value : value);
        address++;

        if (address >= memory->size)
          break;
      }
      if (address >= memory->size)
        break;
    }

    endRender();
  }
};