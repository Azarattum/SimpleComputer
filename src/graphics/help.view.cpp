#include "../coder.cpp"
#include "../memory.cpp"
#include "frame.cpp"

class Helpview : public Frame {
public:
  Helpview(short x, short y, short w, short h) : Frame(x, y, w, h, "Help") {}

  void render() {
    Frame::render();

    printline("q/quit\t\t\ts/save <name>");
    printline("i/counter <val>\tl/load <name>");
    printline("n/next\t\t\te/execute");
    printline("p/prev\t\t\tt/step");
    printline("c/opcode <val>\t\tr/reset");
    printline("o/operand <val>\td/data <val>");
    printline("a/accumulator <val>\tk/kill");

    endRender();
  }
};