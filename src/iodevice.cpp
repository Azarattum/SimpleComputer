#ifndef IODEVICE_GUARD
#define IODEVICE_GUARD
#include <functional>
#include <string.h>

using Word = short;

class IODevice {
public:
  std::function<void(char *)> written_callback;
  std::function<char *(void)> read_callback;

  int read() {
    char *line = read_callback();
    int value = 0;

    const char *hex_chars = "abcdefABCDEF";
    bool ishex = false;
    for (int i = 0; i < strlen(hex_chars); i++) {
      if (strchr(line, hex_chars[i])) {
        ishex = true;
        break;
      }
    }

    if (strlen(line) >= 3 && line[0] == '0' &&
        (line[1] == 'b' || line[1] == 'B')) {
      value = strtol(line + 2, NULL, 2);
    } else if (strlen(line) >= 3 && line[0] == '0' &&
               (line[1] == 'x' || line[1] == 'X')) {
      value = strtol(line + 2, NULL, 16);
    } else if (ishex) {
      value = strtol(line, NULL, 16);
    } else {
      value = strtol(line, NULL, 10);
    }

    return value;
  }

  void write(Word value) {
    char buffer[16];

    bool negative = value < 0;
    value = value < 0 ? -(unsigned)value : value;

    sprintf(buffer, "0x%s%X (%s%d)", negative ? "-" : "", value,
            negative ? "-" : "", value);

    written_callback(buffer);
  }
};

#endif