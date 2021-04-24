#include "compiler.cpp"
#include "machine.cpp"
#include "terminal.cpp"

int main(int argc, char const *argv[]) {
  Machine machine;
  if (argc > 1) {
    if (!strcmp(argv[1], "-a")) {
      if (argc <= 2) {
        printf("Provide an assembly file!\n");
        return 1;
      }

      Word *program = Compiler::compile(argv[2], machine.mem->size);
      for (int i = 0; i < machine.mem->size; i++) {
        (*machine.mem)[i] = program[i];
      }

      delete program;
    } else if (!strcmp(argv[1], "-b")) {
      if (argc <= 2) {
        printf("Provide a simple basic source!\n");
        return 1;
      }
      if (argc <= 3) {
        printf("Provide an output name!\n");
        return 1;
      }

      std::string translated = Compiler::translate(argv[2]);
      FILE *file = fopen(argv[3], "w");
      if (!file)
        return 1;
      fwrite(translated.c_str(), 1, translated.size(), file);
      fclose(file);

      printf("File %s created!\n", argv[3]);
      return 0;
    } else {
      machine.load_program(argv[1]);
    }
  }

  Terminal term(&machine);

  term.render();
  term.interactive();
  return 0;
}
