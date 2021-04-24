#pragma GCC diagnostic ignored "-Wformat-security"
#ifndef FRAME_GUARD
#define FRAME_GUARD

#include <stdio.h>
#include <string.h>

class Frame {
private:
  short contentRow = 0;

protected:
  short x, y, w, h;
  const char *title;

  template <typename... Args>
  void printAt(short x, short y, const char *format = "", Args... args) {
    char buffer[strlen(format) + 32];
    sprintf(buffer, "\033[%d;%dH%s", y + 1, x + 1, format);
    printf(buffer, args...);
  }

  void switchRow(short row) { printAt(x + 2, y + row + 1); }

  template <typename... Args> void printline(const char *format, Args... args) {
    printAt(x + 2, y + ++contentRow, format, args...);
  }

  void endRender() {
    printAt(0, y + h);
    contentRow = 0;
  }

public:
  Frame(short x, short y, short w, short h, const char *title = "")
      : x(x), y(y), w(w), h(h), title(title){};

  virtual void render() {
    short len = strlen(title);
    short left = (w - len - 2) / 2 + (w - len - 2) % 2;
    short right = (w - len - 2) / 2;

    printAt(x, y, "╔");
    while (left--)
      printf("═");
    printf("%s", title);
    while (right--)
      printf("═");
    printf("╗");

    for (int i = 1; i < (h - 1); i++) {
      printAt(x, y + i, "║");
      for (int j = 0; j < (w - 2); j++)
        printf(" ");
      printf("║");
    }

    printAt(x, y + h - 1, "╚");
    short bottom = w - 2;
    while (bottom--)
      printf("═");
    printf("╝");
  }
};

#endif