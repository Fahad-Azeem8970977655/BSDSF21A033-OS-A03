# Makefile - BSDSF21A033 (Features 1–7)

CC       = gcc
CFLAGS   = -Wall -Wextra -g
LDFLAGS  = -lreadline
SRCDIR   = src
BINDIR   = bin
TARGET   = $(BINDIR)/myshell
SRC      = $(SRCDIR)/main.c $(SRCDIR)/builtins.c $(SRCDIR)/history.c $(SRCDIR)/jobs.c
INCLUDE  = -Iinclude

.PHONY: all clean run

all: $(TARGET)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(TARGET): $(SRC) | $(BINDIR)
	@echo "🔧 Compiling myshell (Features 1–7) with Readline support..."
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(LDFLAGS)
	@echo "✅ Build successful! Run with: ./bin/myshell"

run: $(TARGET)
	@./bin/myshell

clean:
	@echo "🧹 Cleaning project..."
	@rm -rf $(BINDIR) *.o
	@echo "✨ Clean complete."

