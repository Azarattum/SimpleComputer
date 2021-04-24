#include "../coder.cpp"
#include "../cpu.cpp"
#include "frame.cpp"

class Cpuview : public Frame {
private:
  void printDevider(short row) {
    printAt(x, y + row + 1, "╠");
    for (int i = 0; i < (w - 2); i++)
      printf("═");
    printf("╣");
  }

public:
  CPU *cpu;

  Cpuview(short x, short y, short w, short h, CPU *cpu)
      : Frame(x, y, w, h, "CPU") {
    this->cpu = cpu;
  }

  void render() {
    Frame::render();

    switchRow(0);
    printf("Accumulator: %s%04X", cpu->ac < 0 ? "-" : "",
           cpu->ac < 0 ? -(unsigned)cpu->ac : cpu->ac);
    printDevider(1);

    switchRow(2);
    printf("Program Counter: %02X", cpu->pc);
    printDevider(3);

    switchRow(4);
    printf("Executed: %s on %02X", Coder::getName((Opcode)(cpu->op)).c_str(),
           cpu->on);
    printDevider(5);

    switchRow(6);
    printf("Flags: ");
    if (cpu->overflow)
      printf("O ");
    if (cpu->zerodevion)
      printf("Z ");
    if (cpu->overmemory)
      printf("M ");
    if (cpu->tickignore)
      printf("T ");
    if (cpu->wrongcommand)
      printf("E ");

    endRender();
  }
};