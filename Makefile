# Makefile - BSDSF21A033 (Feature 1, 2, 3, and 4)

CC       = gcc
CFLAGS   = -Wall -Wextra -g
LDFLAGS  = -lreadline
SRCDIR   = src
BINDIR   = bin
TARGET   = $(BINDIR)/myshell
SRC      = $(SRCDIR)/main.c $(SRCDIR)/builtins.c $(SRCDIR)/history.c
INCLUDE  = -Iinclude

.PHONY: all clean run

# Default target
all: $(TARGET)

# Create bin directory if it doesn't exist
$(BINDIR):
	@mkdir -p $(BINDIR)

# Build the executable
$(TARGET): $(SRC) | $(BINDIR)
	@echo "🔧 Compiling with Readline support..."
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(LDFLAGS)
	@echo "✅ Build successful! Run with: ./bin/myshell"

# Run the shell
run: $(TARGET)
	@./bin/myshell

# Clean build files
clean:
	@echo "🧹 Cleaning project..."
	@rm -rf $(BINDIR) *.o
	@echo "✨ Clean complete."

