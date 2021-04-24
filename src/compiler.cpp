#include "coder.cpp"
#include "expression.cpp"
#include <fstream>
#include <map>
#include <queue>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

using Word = short;

struct parseResult {
  int address;
  Word data;
  Opcode code;
  Word operand;
};

class Compiler {
private:
  static const int dataAddress = 99;
  static const int constAddress = dataAddress - 26;

  static parseResult parseLine(std::string line) {
    std::istringstream stream(line);
    std::string part;

    int index = 0;
    parseResult result;
    while (std::getline(stream, part, ' ')) {
      if (part.empty())
        continue;
      switch (index) {
      case 0:
        result.address = atoi(part.c_str());
        break;
      case 1:
        if (part == "=") {
          result.code = Opcode::END;
          break;
        }

        for (Opcode i = Opcode::NOOP; i < Opcode::END; (*((int *)&i))++) {
          if (Coder::getName(i) == part) {
            result.code = i;
            break;
          }
        }
        break;
      case 2:
        result.operand = (Word)atoi(part.c_str());
        break;

      default:
        return result;
      }
      index++;
    }

    return result;
  }

  static int parseValue(std::string line, std::map<int, int> &constantPool) {
    int address;
    if (line[0] >= 'A' && line[0] <= 'Z') {
      address = dataAddress - (line[0] - 'A');
    } else {
      int data = atoi(line.c_str());
      if (constantPool.find(data) == constantPool.end()) {
        address = constAddress - constantPool.size();
        constantPool[data] = constAddress - constantPool.size();
      } else {
        address = constantPool[data];
      }
    }

    return address;
  }

  static std::string evalExpression(std::queue<std::string> rpn, int &lines,
                                    int &stack,
                                    std::map<int, int> &constantPool) {
    std::string result = "";
    while (!rpn.empty()) {
      std::string token = rpn.front();
      rpn.pop();

      if (Expression::getPrecedence(token[0]) >= 0) {
        result.append(std::to_string(lines++) + " ");
        result.append("LOAD $");
        result.append(std::to_string(--stack - 1) + "\n");

        if (!token.compare("+")) {
          result.append(std::to_string(lines++) + " ");
          result.append("ADD $");
          result.append(std::to_string(stack--) + "\n");
        } else if (!token.compare("*")) {
          result.append(std::to_string(lines++) + " ");
          result.append("MUL $");
          result.append(std::to_string(stack--) + "\n");
        } else if (!token.compare("-")) {
          result.append(std::to_string(lines++) + " ");
          result.append("SUB $");
          result.append(std::to_string(stack--) + "\n");
        } else if (!token.compare("/")) {
          result.append(std::to_string(lines++) + " ");
          result.append("DEVIDE $");
          result.append(std::to_string(stack--) + "\n");
        }

        result.append(std::to_string(lines++) + " ");
        result.append("STORE $");
        result.append(std::to_string(stack++) + "\n");
      } else {
        int address = parseValue(token, constantPool);
        result.append(std::to_string(lines++) + " ");
        result.append("LOAD " + std::to_string(address) + "\n");

        result.append(std::to_string(lines++) + " ");
        result.append("STORE $");
        result.append(std::to_string(stack++) + "\n");
      }
    }

    result.append(std::to_string(lines++) + " ");
    result.append("LOAD $" + std::to_string(--stack) + "\n");

    return result;
  }

  static std::string parseCode(std::string line, int &lines, int &stack,
                               std::map<int, int> &addressMap,
                               std::map<int, int> &constantPool) {
    int lineNumber = atoi(line.c_str());
    addressMap[lineNumber] = lines;
    if (line.find("REM") != std::string::npos)
      return std::string("");

    std::string translated = "";
    translated.append(std::to_string(lines++));
    translated.append(" ");

    if (line.find("IF ") != std::string::npos) {
      line.erase(line.begin(), line.begin() + line.find("IF") + 2);
      eraseAll(line, " ");

      int addressLeft = parseValue(line, constantPool);
      eraseUntil(line, "<=>");

      enum Type { Equal, Less, More };
      Type operation;
      switch (line[0]) {
      case '=':
        operation = Equal;
        break;
      case '<':
        operation = Less;
        break;
      case '>':
        operation = More;
        break;
      default:
        throw std::logic_error(
            (std::string("Invalid comparation operation on line ") +
             std::to_string(lineNumber))
                .c_str());
        break;
      }
      eraseAll(line, " <=>");
      int addressRight = parseValue(line, constantPool);
      eraseUntil(line, " ");
      eraseAll(line, " ");

      switch (operation) {
      case Less: {
        translated.append("LOAD ");
        translated.append(std::to_string(addressLeft));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" SUB ");
        translated.append(std::to_string(addressRight));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" JNEG ");
        translated.append(std::to_string(lines + 1));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" JUMP ");

        std::string inner =
            parseCode(line, lines, stack, addressMap, constantPool);

        translated.append(std::to_string(lines));
        translated.append("\n");

        translated.append(inner);
        break;
      }
      case More: {
        translated.append("NEG ");
        translated.append(std::to_string(addressLeft));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" ADD ");
        translated.append(std::to_string(addressRight));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" JNEG ");
        translated.append(std::to_string(lines + 1));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" JUMP ");

        std::string inner =
            parseCode(line, lines, stack, addressMap, constantPool);

        translated.append(std::to_string(lines));
        translated.append("\n");

        translated.append(inner);
        break;
      }
      case Equal: {
        translated.append("LOAD ");
        translated.append(std::to_string(addressLeft));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" SUB ");
        translated.append(std::to_string(addressRight));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" JZ ");
        translated.append(std::to_string(lines + 1));
        translated.append("\n");

        translated.append(std::to_string(lines++));
        translated.append(" JUMP ");

        std::string inner =
            parseCode(line, lines, stack, addressMap, constantPool);

        translated.append(std::to_string(lines));
        translated.append("\n");

        translated.append(inner);
        break;
      }
      }
    }

    else if (line.find("LET ") != std::string::npos) {
      line.erase(line.begin(), line.begin() + line.find("LET") + 3);
      eraseAll(line, " ");

      int dest = parseValue(line, constantPool);
      eraseUntil(line, "=");
      eraseAll(line, " =");

      char buffer[strlen(line.c_str())];
      strcpy(buffer, line.c_str());
      auto rpn = Expression::toRPN(buffer);
      translated = "";
      lines--;
      translated.append(evalExpression(rpn, lines, stack, constantPool));

      translated.append(std::to_string(lines++) + " ");
      translated.append("STORE ");
      translated.append(std::to_string(dest));
    }

    else if (line.find("INPUT ") != std::string::npos) {
      int address = dataAddress - (line[line.size() - 1] - 'A');

      translated.append("READ ");
      translated.append(std::to_string(address));
    }

    else if (line.find("PRINT ") != std::string::npos) {
      line.erase(line.begin(), line.begin() + line.find("PRINT") + 5);
      eraseAll(line, " ");
      int address = parseValue(line, constantPool);

      translated.append("WRITE ");
      translated.append(std::to_string(address));
    }

    else if (line.find("GOTO ") != std::string::npos) {
      int address = atoi(strstr(line.c_str(), "GOTO ") + 5);

      translated.append("JUMP ");
      translated.append("%" + std::to_string(address));
    }

    else if (line.find("END") != std::string::npos) {
      translated.append("HALT");
    }

    if (translated[translated.length() - 1] != '\n')
      translated.append("\n");
    return translated;
  }

  static void replaceAll(std::string &str, const std::string &from,
                         const std::string &to) {
    if (from.empty())
      return;
    int startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
      str.replace(startPos, from.length(), to);
      startPos += to.length();
    }
  }

  static void eraseAll(std::string &line, const char *chars) {
    while (strchr(chars, line[0]))
      line.erase(line.begin(), line.begin() + 1);
  }

  static void eraseUntil(std::string &line, const char *chars) {
    while (!strchr(chars, line[0]))
      line.erase(line.begin(), line.begin() + 1);
  }

public:
  static Word *compile(const char *filename, int memorySize) {
    Word *buffer = new Word[memorySize];
    for (int i = 0; i < memorySize; i++)
      buffer[i] = 0;

    std::ifstream file(filename);
    std::string str;
    while (std::getline(file, str)) {
      parseResult result = parseLine(str);
      // Data check
      if (result.code == Opcode::END) {
        buffer[result.address] = result.operand;
      } else {
        buffer[result.address] = Coder::encode(result.code, result.operand);
      }
    }

    return buffer;
  }

  static std::string translate(const char *filename) {
    std::map<int, int> addressMap;
    std::map<int, int> constantPool;

    std::ifstream file(filename);
    std::string translated = "";
    std::string line;
    int lines = 0;
    int stack = 0;

    while (std::getline(file, line)) {
      translated.append(
          parseCode(line, lines, stack, addressMap, constantPool));
    }

    for (const auto &[key, value] : constantPool) {
      translated.append(std::to_string(value));
      translated.append(" = ");
      translated.append(std::to_string(key));
      translated.append("\n");
    }

    for (const auto &[key, value] : addressMap) {
      replaceAll(translated, "%" + std::to_string(key), std::to_string(value));
    }

    for (int i = 0; i < dataAddress; i++) {
      replaceAll(translated, "$" + std::to_string(i),
                 std::to_string(lines + i));
    }

    return translated;
  }
};