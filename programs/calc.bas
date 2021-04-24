10  REM Simple calculator
20  INPUT A
30  INPUT B
40  INPUT D
60  IF D = 1 LET C = A + B
70  IF D = 2 LET C = A - B
80  IF D = 3 LET C = A * B
90  IF D = 4 LET C = A / B
10  IF D > 4 GOTO 40
11  IF D < 1 GOTO 40
12  PRINT C
13  END