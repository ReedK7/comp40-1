#!/bin/sh

make all

printf "   *****   RUNNING TESTS   *****\n\n"

./IO_test < allASCIIchars.txt > /dev/null
printf "\n"
./ProgramLoader_test
printf "\n"
./Registers_test
printf "\n"
./MemorySegments_test
printf "\n"
./Machine_test < allASCIIchars.txt > /dev/null
printf "\n"

printf "   *****       DONE       *****\n"
