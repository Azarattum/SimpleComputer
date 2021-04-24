# Simple Computer
A simple computer emulator.

## Features:
  - TUI with memory and CPU view
  - Interactive memory editing
  - Step by step evaluation
  - Simple assembly support
  - Compilation from simple basic to simple assembly

## Installation
To compile the program
```sh
git clone https://github.com/Azarattum/SimpleComputer
cd SimpleComputer
make release
```
To compile all the example programs
```sh
make programs
```

## Usage:
Run emulator
```sh
sc
```

Run compiled binary
```sh
sc program.bin
```

Run assembly file
```sh
sc -a program.asm
```

Compile simple basic to an assembly file
```sh
sc -b program.bas program.asm
```

## TUI
When running in terminal the program looks like:
```
╔══════════════════════════════════Memory═════════════════════════════════╗ ╔══════════════CPU═════════════╗
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ║ Accumulator: 0000            ║
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ╠══════════════════════════════╣
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ║ Program Counter: 00          ║
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ╠══════════════════════════════╣
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ║ Executed: NOOP on 00         ║
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ╠══════════════════════════════╣
║ +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 +0000 ║ ║ Flags:                       ║
╚═════════════════════════════════════════════════════════════════════════╝ ╚══════════════════════════════╝
╔═════════════════════════════════════╗╔═══════════I/O═══════════╗╔══════════════════Help══════════════════╗
║                                     ║║                         ║║ q/quit                 s/save <name>   ║
║         ▒▒▒▒▒  ▒▒▒▒▒  ▒▒▒▒▒  ▒▒▒▒▒  ║║                         ║║ i/counter <val>        l/load <name>   ║
║    ▒    ▒   ▒  ▒   ▒  ▒   ▒  ▒   ▒  ║║                         ║║ n/next                 e/execute       ║
║  ▒▒▒▒▒  ▒   ▒  ▒   ▒  ▒   ▒  ▒   ▒  ║║                         ║║ p/prev                 t/step          ║
║    ▒    ▒   ▒  ▒   ▒  ▒   ▒  ▒   ▒  ║║                         ║║ c/opcode <val>         r/reset         ║
║         ▒▒▒▒▒  ▒▒▒▒▒  ▒▒▒▒▒  ▒▒▒▒▒  ║║                         ║║ o/operand <val>        d/data <val>    ║
║                                     ║║                         ║║ a/accumulator <val>    k/kill          ║
╚═════════════════════════════════════╝╚═════════════════════════╝╚════════════════════════════════════════╝
> 
```

## Supported Opcodes
*The code numbers are given in hexadecimal.*

| Operation | Code | Descrition                                                  |
| --------- | ---- | ----------------------------------------------------------- |
| READ      | 10   | Save input value to an address                              |
| WRITE     | 11   | Print from an address                                       |
| LOAD      | 20   | Load to the accumulator from an address                     |
| STORE     | 21   | Save accumulator value to an address                        |
| ADD       | 30   | Add to the accumulator from an address                      |
| SUB       | 31   | Subtract from the accumulator an address value              |
| DIVIDE    | 32   | Devide the accumulator by an address value                  |
| MUL       | 33   | Multiply the accumulator by an address value                |
| JUMP      | 40   | Move execution to an address                                |
| JNEG      | 41   | Move execution to an address (when accumulator is negative) |
| JZ        | 42   | Move execution to an address (when accumulator is zero)     |
| HALT      | 43   | Stop the program execution                                  |
| NEG       | 64   | Load to the accumulator inverted value from an address      |

## Simple Basic
The compiler supports simple basic syntax. The supported operators are: REM, INPUT, PRINT, GOTO, IF, LET, END.

An example code:
```bas
10 REM I am a comment
20 INPUT A
30 INPUT B
40 LET C = (A - B) * 2
50 IF C < 0 GOTO 20
60 PRINT C
70 END
```

For more examples check out *./programs* directory.