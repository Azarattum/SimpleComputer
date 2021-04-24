#include "string.h"
#include <queue>
#include <stack>
#include <string>

class Expression {
  static bool isSymbol(char symbol) {
    const char *symbols = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
    return strchr(symbols, symbol);
  }

public:
  static int getPrecedence(int op) {
    switch (op) {
    case '+':
      return 0;
    case '-':
      return 0;
    case '*':
      return 1;
    case '/':
      return 1;

    default:
      return -1;
    }
  }

  static std::queue<std::string> toRPN(char *expr) {
    std::queue<std::string> tokens;

    std::stack<char> operators;

    while (*expr) {
      if (*expr == ' ' || *expr == '\t') {
        expr++;
        continue;
      } else if (isSymbol(*expr)) {
        int length = 0;
        while (expr[length] && isSymbol(expr[length]))
          length++;

        tokens.push(std::string(expr, length));
        expr += length;
      } else {
        switch (*expr) {
        case '(':
          operators.push('(');
          expr++;
          break;
        case ')':
          while (operators.top() != '(') {
            tokens.push(std::string(1, operators.top()));
            operators.pop();
          }
          operators.pop();
          expr++;
          break;
        default: {
          while (operators.size() &&
                 getPrecedence(operators.top()) >= getPrecedence(*expr)) {
            tokens.push(std::string(1, operators.top()));
            operators.pop();
          }
          operators.push(*expr);
          expr++;
        }
        }
      }
    }

    while (!operators.empty()) {
      tokens.push(std::string(1, operators.top()));
      operators.pop();
    }

    return tokens;
  }
};