CFLAGS = -ansi -pedantic -g -Wall

# Source files
SRC = $(wildcard C_Files/*.c)

# Output executable
OUT = Output_Files/assembler

# Target to build the assembler executable
$(OUT): $(SRC)
	gcc $(CFLAGS) $(SRC) -o $(OUT)
