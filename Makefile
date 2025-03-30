# Makefile for Racecourse Management System

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c99

# Source files
SRC_DIR = src
INCLUDE_DIR = includes
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

TEST_DIR = tests
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

TEST_DIR = tests
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_OBJ_FILES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)

# Object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_OBJ_FILES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)

# Build directory
BUILD_DIR = build

# Output executable
OUTPUT = bin/racecourse

TEST_OUTPUTS = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

SQLITE_LIB = -lsqlite3
CHECK_LIB = -lcheck -lm -lpthread -lsubunit

all: $(OUTPUT) $(TEST_OUTPUTS)

# Build the output executable
$(OUTPUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB)

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES))
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB) $(CHECK_LIB)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

test: $(TEST_OUTPUTS)
	for test in $(TEST_OUTPUTS); do $$test; done

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT) $(TEST_OUTPUTS)

# Run tests
test: $(TEST_OUTPUTS)
	for test in $(TEST_OUTPUTS); do $$test; done

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT) $(TEST_OUTPUTS)

# Print help message
help:
	@echo "Makefile for Racecourse Management System"
	@echo "Usage:"
	@echo "  make                - Build the project"
	@echo "  make test           - Run tests"
	@echo "  make clean          - Clean up build artifacts"
	@echo "  make help           - Show this help message"

.PHONY: all clean help test
