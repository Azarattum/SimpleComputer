#include "../coder.cpp"
#include "../cpu.cpp"
#include "../memory.cpp"
#include "frame.cpp"

// ▒▒▒▒▒ 0
// ▒   ▒ 3,4  ▒
// ▒▒▒▒▒ 1    ▒ 7
// ▒   ▒ 5,6  ▒
// ▒▒▒▒▒ 2

const short Patterns[] = {
    0b0010111110, // 0
    0b0000001010, // 1
    0b0011101100, // 2
    0b0011101010, // 3
    0b0001011010, // 4
    0b0011110010, // 5
    0b0011110110, // 6
    0b0010001010, // 7
    0b0011111110, // 8
    0b0011111010, // 9
    0b1011011110, // A
    0b1111111110, // B
    0b0010110100, // C
    0b1110111110, // D
    0b0011110100, // E
    0b0011010100, // F
    0b0001000000, // -
    0b0001000001, // +
};

class Bigview : public Frame {
private:
  void printBig(short x, short y, unsigned char num) {
    short pattern = Patterns[num];

    if (pattern & 0b10000000) {
      printAt(x, y, "▒▒▒▒▒");
    }
    if (pattern & 0b01000000) {
      printAt(x, y + 2, "▒▒▒▒▒");
    }
    if (pattern & 0b00100000) {
      printAt(x, y + 4, "▒▒▒▒▒");
    }
    if (pattern & 0b00010000) {
      printAt(x, y, "▒");
      printAt(x, y + 1, "▒");
      printAt(x, y + 2, "▒");
    }
    if (pattern & 0b00001000) {
      printAt(x + 4, y, "▒");
      printAt(x + 4, y + 1, "▒");
      printAt(x + 4, y + 2, "▒");
    }
    if (pattern & 0b00000100) {
      printAt(x, y + 2, "▒");
      printAt(x, y + 2 + 1, "▒");
      printAt(x, y + 2 + 2, "▒");
    }
    if (pattern & 0b00000010) {
      printAt(x + 4, y + 2, "▒");
      printAt(x + 4, y + 2 + 1, "▒");
      printAt(x + 4, y + 2 + 2, "▒");
    }
    if (pattern & 0b00000001) {
      printAt(x + 2, y + 1, "▒");
      printAt(x + 2, y + 2, "▒");
      printAt(x + 2, y + 3, "▒");
    }
    if (pattern & 0b1000000000) {
      printAt(x + 4, y, " ");
    }
    if (pattern & 0b0100000000) {
      printAt(x + 4, y + 4, " ");
    }
  }

public:
  Memory *memory;
  CPU *cpu;

  Bigview(short x, short y, short w, short h, CPU *cpu, Memory *memory)
      : Frame(x, y, w, h, "") {
    this->cpu = cpu;
    this->memory = memory;
  }

  void render() {
    Frame::render();

    short margin = (w - (2 + 7 * 5)) / 2;

    try {
      Word selected = (*memory)[cpu->pc];

      short digit = 16 + (selected >= 0);
      printBig(margin + x + 2, y + 2, digit);

      selected = selected < 0 ? -(unsigned)selected : selected;

      digit = (selected & 0xF000) >> 12;
      printBig(margin + x + 2 + 7 * 1, y + 2, digit);
      digit = (selected & 0x0F00) >> 8;
      printBig(margin + x + 2 + 7 * 2, y + 2, digit);
      digit = (selected & 0x00F0) >> 4;
      printBig(margin + x + 2 + 7 * 3, y + 2, digit);
      digit = selected & 0x000F;
      printBig(margin + x + 2 + 7 * 4, y + 2, digit);
    } catch (...) {
      for (int i = 0; i < 5; i++)
        printBig(margin + x + 2 + 7 * i, y + 2, 16);
    }

    endRender();
  }
};