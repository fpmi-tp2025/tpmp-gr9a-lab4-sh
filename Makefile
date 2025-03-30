CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c99

SRC_DIR = src
INCLUDE_DIR = includes
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

BUILD_DIR = build

OUTPUT = bin/racecourse

SQLITE_LIB = -lsqlite3

all: $(OUTPUT)

$(OUTPUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT)

help:
	@echo "Makefile for Racecourse Management System"
	@echo "Usage:"
	@echo "  make                - Build the project"
	@echo "  make clean          - Clean up build artifacts"
	@echo "  make help           - Show this help message"

.PHONY: all clean help
