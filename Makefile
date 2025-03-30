# Makefile for Racecourse Management System

# ����������
CC = gcc

# ����� ���������� (��������� ��������)
CFLAGS = -Wall -Wextra -Werror -std=c99 --coverage

# ����� ������ ���������� (���� ��������� ��������)
LDFLAGS = --coverage

# �����
SRC_DIR = src
INCLUDE_DIR = includes
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

# SQLite � Check
SQLITE_LIB = -lsqlite3
CHECK_LIB = -lcheck -lm -lpthread -lsubunit

# ��������� *.c �� src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# �������� *.c
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# ����������� ����� .c -> .o
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_OBJ_FILES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)

# �������� ����������� ���� ����������
APP_EXEC = $(BIN_DIR)/racecourse

# ����������� ����� ������ (�������� ���� ����� ��, ��� ��� �������� ������ ��� .c)
TEST_EXECUTABLES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

# ���� �� ���������
all: $(APP_EXEC) $(TEST_EXECUTABLES)

# ������� ������ ��������� ����������
$(APP_EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB) $(LDFLAGS)

# ������� ������ ��������� ������ �� src/*.c
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# ������� ������ ��������� ������������ �����, �������� build/test_auth
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB) $(CHECK_LIB) $(LDFLAGS)

# ������� ������ ����������� �� tests/*.c
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# ������ ���� ������
test: $(TEST_EXECUTABLES)
	@echo "Running tests..."
	@for t in $(TEST_EXECUTABLES); do \
		echo "===== Running $$t ====="; \
		$$t || exit 1; \
	done
	@echo "All tests passed."

# ��������� ������ �� �������� ��� ������ lcov + genhtml
coverage: test
	@echo "Generating coverage report..."
	@lcov --directory . --capture --output-file coverage.info 2>/dev/null
	@lcov --remove coverage.info '/usr/*' --output-file coverage.info 2>/dev/null
	@genhtml coverage.info --output-directory coverage 2>/dev/null
	@echo "Coverage report generated in ./coverage/index.html"

# ���� ������������� gcovr:
# coverage:
# 	make test
# 	gcovr -r . --html --html-details -o coverage.html

# �������
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) coverage *.info *.gcda *.gcno

# ����� ���������
help:
	@echo "Makefile for Racecourse Management System"
	@echo "Usage:"
	@echo "  make            - Build the project"
	@echo "  make test       - Build and run tests"
	@echo "  make coverage   - Generate coverage report (needs lcov & genhtml)"
	@echo "  make clean      - Clean up build artifacts"
	@echo "  make help       - Show this help message"

.PHONY: all clean help test coverage
