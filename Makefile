# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# Libraries
LIBS = -lcurl -ljansson

# Target program
TARGET = main

# Default rule
all: $(TARGET)

# Build target
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(LIBS)

# Clean up build files
clean:
	rm -f $(TARGET)
