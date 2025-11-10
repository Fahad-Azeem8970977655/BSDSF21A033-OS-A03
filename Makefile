# 🧠 Makefile - BSDSF21A033 (Features 1, 2, 3, 4, 5, and 6)
# Author: Fahad Azeem
# Description:
#   Builds the custom mini shell supporting:
#   - Feature 1: Base Shell
#   - Feature 2: Built-in Commands
#   - Feature 3: Command History & !n
#   - Feature 4: GNU Readline Integration
#   - Feature 5: I/O Redirection & Single Pipe
#   - Feature 6: Background Job Control

CC       = gcc
CFLAGS   = -Wall -Wextra -g
LDFLAGS  = -lreadline
SRCDIR   = src
BINDIR   = bin
TARGET   = $(BINDIR)/myshell
SRC      = $(SRCDIR)/main.c \
           $(SRCDIR)/builtins.c \
           $(SRCDIR)/history.c \
           $(SRCDIR)/jobs.c
INCLUDE  = -Iinclude

.PHONY: all clean run

# 🔹 Default target
all: $(TARGET)

# 🔹 Ensure bin directory exists
$(BINDIR):
	@mkdir -p $(BINDIR)

# 🔹 Build the shell executable
$(TARGET): $(SRC) | $(BINDIR)
	@echo "🔧 Compiling myshell (Features 1–6) with Readline support..."
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(LDFLAGS)
	@echo "✅ Build successful! Run with: ./bin/myshell"

# 🔹 Run the shell interactively
run: $(TARGET)
	@./bin/myshell

# 🔹 Clean build files
clean:
	@echo "🧹 Cleaning project..."
	@rm -rf $(BINDIR) *.o
	@echo "✨ Clean complete."

