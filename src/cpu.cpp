#ifndef CPU_GUARD
#define CPU_GUARD
#include "iodevice.cpp"
#include "memory.cpp"
#include <readline/readline.h>

using Byte = unsigned char;
using Word = short;

class CPU {
public:
  // Registers
  Word ac; // Accumulator
  Byte pc; // Program counter
  Byte op; // Last operation
  Byte on; // Last operand
  // Flags
  Byte overflow : 1;
  Byte zerodevion : 1;
  Byte overmemory : 1;
  Byte tickignore : 1;
  Byte wrongcommand : 1;

  void reset() {
    ac = 0x0;
    pc = 0x0;
    overflow = zerodevion = overmemory = tickignore = wrongcommand = 0;
  }

  bool tick(Memory *mem, IODevice *io) {
    if (tickignore)
      return false;

    try {
      Command command = Coder::decode((*mem)[pc]);
      if (!command.valid)
        wrongcommand = 1;

      if (wrongcommand ||
          !execute(command.opcode, command.operand, *mem, *io)) {
        tickignore = 1;
        return false;
      }
      op = command.opcode;
      on = command.operand;
    } catch (...) {
      tickignore = 1;
      overmemory = 1;
      return false;
    }

    pc++;
    if (pc >= mem->size)
      overmemory = 1;

    return true;
  }

private:
  bool execute(Opcode opcode, Word operand, Memory &mem, IODevice &io) {
    switch (opcode) {
    case Opcode::NOOP:
      break;
    case Opcode::READ: {
      int value = io.read();
      if (value > 0x7FFF || value < -0x8000) {
        overflow = 1;
        return false;
      }
      mem[operand] = (Word)value;
    } break;
    case Opcode::WRITE:
      try {
        Word value = mem[operand];
        io.write(value);
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::LOAD:
      try {
        ac = mem[operand];
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::STORE:
      try {
        mem[operand] = ac;
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::ADD:
      try {
        if ((int)ac + mem[operand] > 0x7FFF) {
          overflow = 1;
          return false;
        }
        ac += mem[operand];
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::SUB:
      try {
        if ((int)ac - mem[operand] < -0x8000) {
          overflow = 1;
          return false;
        }
        ac -= mem[operand];
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::DEVIDE:
      try {
        if (mem[operand] == 0) {
          zerodevion = 1;
          return false;
        }
        ac /= mem[operand];
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::MUL:
      try {
        if ((int)ac * mem[operand] > 0x7FFF) {
          overflow = 1;
          return false;
        }
        ac *= mem[operand];
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;
    case Opcode::JUMP:
      if (operand >= mem.size || operand < 0) {
        overmemory = 1;
        return false;
      }
      pc = operand - 1;
      break;
    case Opcode::JNEG:
      if (ac >= 0)
        return true;
      if (operand >= mem.size || operand < 0) {
        overmemory = 1;
        return false;
      }
      pc = operand - 1;
      break;
    case Opcode::JZ:
      if (ac)
        return true;
      if (operand >= mem.size || operand < 0) {
        overmemory = 1;
        return false;
      }
      pc = operand - 1;
      break;
    case Opcode::HALT:
      return false;
      break;
    case Opcode::NEG:
      try {
        ac = -mem[operand];
      } catch (...) {
        overmemory = 1;
        return false;
      }
      break;

    default:
      wrongcommand = 1;
      return false;
      break;
    }

    return true;
  }
};

#endif