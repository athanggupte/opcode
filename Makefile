CC=gcc-10
CFLAGS=-O
LDFLAGS=-lm

# .SILENT:
.PHONY: all clean

all: opcode_shell float_bytes

opcode_shell: opcode_shell.o opcode.o

opcode_shell.o: opcode_shell.c opcode.h

opcode.o: opcode.c opcode.h

# opcode.h: opcodes.generated.inl

# opcode.generated.inl: opcodes.def
#	python3 generate_opcodes.py opcodes opcodes.def > opcodes.generated.inl

# tools
float_bytes: float_bytes.c

# utils
clean:
	rm -f opcode_shell *.o *.generated.*

