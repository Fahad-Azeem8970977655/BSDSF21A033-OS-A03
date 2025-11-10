# 🧠 Makefile - BSDSF21A033 (Features 1–6)
# Author: Fahad Azeem
# Description:
#   Build configuration for the Mini Shell project supporting:
#   1️⃣ Base Shell (fork, exec)
#   2️⃣ Built-in Commands (cd, help, exit, history, jobs)
#   3️⃣ Command History & !n Re-execution
#   4️⃣ GNU Readline Integration
#   5️⃣ I/O Redirection and Single Piping
#   6️⃣ Background Job Control (&, jobs)

# Compiler settings
CC       = gcc
CFLAGS   = -Wall -Wextra -g
LDFLAGS  = -lreadline
INCLUDE  = -Iinclude

# Folder structure
SRCDIR   = src
BINDIR   = bin
TARGET   = $(BINDIR)/myshell

# Source files
SRC = $(SRCDIR)/main.c \
      $(SRCDIR)/builtins.c \
      $(SRCDIR)/history.c \
      $(SRCDIR)/jobs.c

# Phony targets (not real files)
.PHONY: all clean run

# Default build target
all: $(TARGET)

# Ensure output directory exists
$(BINDIR):
	@mkdir -p $(BINDIR)

# Build executable
$(TARGET): $(SRC) | $(BINDIR)
	@echo "🔧 Compiling myshell (Features 1–6) with Readline support..."
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(LDFLAGS)
	@echo "✅ Build successful! Run with: ./bin/myshell"

# Run the shell interactively
run: $(TARGET)
	@./bin/myshell

# Clean compiled output
clean:
	@echo "🧹 Cleaning project..."
	@rm -rf $(BINDIR) *.o
	@echo "✨ Clean complete."

