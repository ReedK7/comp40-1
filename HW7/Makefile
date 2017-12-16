#
# Makefile
# Ethan Chen and Genevieve Silverman
# HW 6 - UM
#
# compile project through command: $ make all
# 

CC = gcc

IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic -O2 $(IFLAGS)
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64
LDLIBS  = -lcii40-O2 -l40locality -lcii40 -lm

EXECS   = um

all: $(EXECS)

um: UM.o Machine.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

