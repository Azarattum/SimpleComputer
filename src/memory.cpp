#ifndef MEMORY_GUARD
#define MEMORY_GUARD
#include <stdexcept>

using Word = short;
using Byte = unsigned char;

class Memory {
private:
  Word *data;

public:
  int size;

  Memory(int size = 100) {
    data = new Word[size];
    this->size = size;
  }
  ~Memory() { delete data; }

  Word &operator[](Byte address) {
    if (address >= size || address < 0)
      throw std::overflow_error("Out of memory bounds!");
    return data[address];
  }

  void reset() {
    for (int i = 0; i < size; i++)
      data[i] = 0;
  }

  void dump(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file)
      return;
    fwrite(data, sizeof(Word), size, file);
    fclose(file);
  }

  void load(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file)
      return;
    fread(data, sizeof(Word), size, file);
    fclose(file);
  }
};

#endif