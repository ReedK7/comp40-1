#
# Makefile
# Ethan Chen and Genevieve Silverman
# HW 6 - UM
#
# compile project through command: $ make all
# 

CC = gcc

IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64
LDLIBS  = -lcii40-O2 -l40locality -lcii40 -lm

EXECS   = writetests MemorySegments_test IO_test Registers_test \
	  ProgramLoader_test Machine_test um

all: $(EXECS)

writetests: umlabwrite.o umlab.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

IO_test: IO_test.o IO.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

MemorySegments_test: MemorySegments_test.o MemorySegments.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

Registers_test: Registers.o Registers_test.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

ProgramLoader_test: ProgramLoader.o ProgramLoader_test.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

Machine_test: Machine_test.o Machine.o bitpack.o Registers.o \
	MemorySegments.o IO.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um: UM.o Machine.o bitpack.o Registers.o MemorySegments.o IO.o ProgramLoader.o
	$(CC) -O2 $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

