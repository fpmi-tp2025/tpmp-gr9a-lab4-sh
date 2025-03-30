# Makefile for Racecourse Management System

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c99

# Source files
SRC_DIR = src
INCLUDE_DIR = includes
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Build directory
BUILD_DIR = build

# Output executable
OUTPUT = bin/racecourse

# SQLite library
SQLITE_LIB = -lsqlite3

# Default target
all: $(OUTPUT)

# Build the output executable
$(OUTPUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT)

# Print help message
help:
	@echo "Makefile for Racecourse Management System"
	@echo "Usage:"
	@echo "  make                - Build the project"
	@echo "  make clean          - Clean up build artifacts"
	@echo "  make help           - Show this help message"

.PHONY: all clean help
