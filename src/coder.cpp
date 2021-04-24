#ifndef CODER_GUARD
#define CODER_GUARD
#include <string>
using Byte = unsigned char;
using Word = short;

enum Opcode {
  NOOP = 0x0,
  // IO
  READ = 0x10,
  WRITE = 0x11,
  // Accumulator
  LOAD = 0x20,
  STORE = 0x21,
  // Arithmetics
  ADD = 0x30,
  SUB = 0x31,
  DEVIDE = 0x32,
  MUL = 0x33,
  // Control
  JUMP = 0x40,
  JNEG = 0x41,
  JZ = 0x42,
  HALT = 0x43,
  // User funcs
  NOT = 0x51,
  AND = 0x52,
  OR = 0x53,
  XOR = 0x54,
  JNS = 0x55,
  JC = 0x56,
  JNC = 0x57,
  JP = 0x58,
  JNP = 0x59,
  CHL = 0x60,
  SHR = 0x61,
  RCL = 0x62,
  RCR = 0x63,
  NEG = 0x64,
  ADDC = 0x65,
  SUBC = 0x66,
  LOGLC = 0x67,
  LOGRC = 0x68,
  RCCL = 0x69,
  RCCR = 0x70,
  MOVA = 0x71,
  MOVR = 0x72,
  MOVCA = 0x73,
  MOVCR = 0x74,
  ADDC2 = 0x75,
  SUBC2 = 0x76,
  END
};

struct Command {
  bool valid;
  Opcode opcode;
  Byte operand;
};

class Coder {
public:
  static Command decode(Word command) {
    Command decoded;
    decoded.valid = !(command & 0b1000000000000000);
    decoded.opcode = (Opcode)((command & 0b1111111100000000) >> 8);
    decoded.operand = command & 0b0000000011111111;
    return decoded;
  }

  static Word encode(Opcode opcode, Word operand) {
    Word encoded = (Word)(opcode << 8);
    encoded |= operand;

    return encoded;
  }

  static Word encode(Command command) {
    return encode(command.opcode, command.operand);
  }

  static std::string getName(Opcode opcode) {
    switch (opcode) {
    case 0x0:
      return "NOOP";
    case 0x10:
      return "READ";
    case 0x11:
      return "WRITE";
    case 0x20:
      return "LOAD";
    case 0x21:
      return "STORE";
    case 0x30:
      return "ADD";
    case 0x31:
      return "SUB";
    case 0x32:
      return "DEVIDE";
    case 0x33:
      return "MUL";
    case 0x40:
      return "JUMP";
    case 0x41:
      return "JNEG";
    case 0x42:
      return "JZ";
    case 0x43:
      return "HALT";
    case 0x51:
      return "NOT";
    case 0x52:
      return "AND";
    case 0x53:
      return "OR";
    case 0x54:
      return "XOR";
    case 0x55:
      return "JNS";
    case 0x56:
      return "JC";
    case 0x57:
      return "JNC";
    case 0x58:
      return "JP";
    case 0x59:
      return "JNP";
    case 0x60:
      return "CHL";
    case 0x61:
      return "SHR";
    case 0x62:
      return "RCL";
    case 0x63:
      return "RCR";
    case 0x64:
      return "NEG";
    case 0x65:
      return "ADDC";
    case 0x66:
      return "SUBC";
    case 0x67:
      return "LOGLC";
    case 0x68:
      return "LOGRC";
    case 0x69:
      return "RCCL";
    case 0x70:
      return "RCCR";
    case 0x71:
      return "MOVA";
    case 0x72:
      return "MOVR";
    case 0x73:
      return "MOVCA";
    case 0x74:
      return "MOVCR";
    case 0x75:
      return "ADDC2";
    case 0x76:
      return "SUBC2";
    default:
      return "";
    }
  }
};

#endif