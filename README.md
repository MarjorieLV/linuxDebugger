Linux debbuger for linux x86

## Caution
Some commands will not work outsite of program file

## To build
cmake .
make

## To launch
./linbug.exe <fileName>
./linbug.exe program.exe

## Commands
- c to continue

- b to break
    b 0x<address>
    b <funcName>
    b <filename>:<line>
  
- reg to do action on registers
    reg write <registerName> <value>
    reg read <registerName>
    reg dump

- mem to do action on memory
    mem read <address>
    mem write <address> <value>

- s to step

- n to next

- finish to step out scope

- symbol to find symbol
    symbol <arg>
    
- bt to backtrace
