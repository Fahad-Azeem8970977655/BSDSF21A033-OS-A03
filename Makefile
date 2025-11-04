# Makefile - BSDSF21A033 (Feature 1, 2, and 3)

CC = gcc
CFLAGS = -Wall -Wextra -g
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/myshell
SRC = $(SRCDIR)/main.c $(SRCDIR)/builtins.c $(SRCDIR)/history.c

.PHONY: all clean

all: $(TARGET)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(TARGET): $(SRC) | $(BINDIR)
	@echo "Compiling shell..."
	$(CC) $(CFLAGS) -Iinclude -o $@ $(SRC)
	@echo "Build successful! Run ./bin/myshell"

clean:
	@echo "Cleaning up..."
	@rm -rf $(BINDIR) *.o
	@echo "Done."

