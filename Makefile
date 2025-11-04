# BSDSF21A033 - Feature 1 Base Shell Makefile

CC = gcc
CFLAGS = -Wall -Wextra -g
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/myshell
SRC = $(SRCDIR)/main.c $(SRCDIR)/builtins.c


.PHONY: all clean

all: $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(SRC) | $(BINDIR)
	$(CC) $(CFLAGS) -Iinclude -o $@ $(SRC)

clean:
	rm -rf $(BINDIR) *.o
