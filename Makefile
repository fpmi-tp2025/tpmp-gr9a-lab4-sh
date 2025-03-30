# Makefile for Racecourse Management System

# Компилятор
CC = gcc

# Флаги компиляции (добавляем покрытие)
CFLAGS = -Wall -Wextra -Werror -std=c99 --coverage

# Флаги сборки линковщика (тоже добавляем покрытие)
LDFLAGS = --coverage

# Папки
SRC_DIR = src
INCLUDE_DIR = includes
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

# SQLite и Check
SQLITE_LIB = -lsqlite3
CHECK_LIB = -lcheck -lm -lpthread -lsubunit

# Исходники *.c из src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# Тестовые *.c
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Преобразуем имена .c -> .o
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_OBJ_FILES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)

# Итоговый исполняемый файл приложения
APP_EXEC = $(BIN_DIR)/racecourse

# Исполняемые файлы тестов (названия берём такие же, как имя тестовых файлов без .c)
TEST_EXECUTABLES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

# Цель по умолчанию
all: $(APP_EXEC) $(TEST_EXECUTABLES)

# Правило сборки основного приложения
$(APP_EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB) $(LDFLAGS)

# Правило сборки объектных файлов из src/*.c
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Правило сборки тестового исполняемого файла, например build/test_auth
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(SQLITE_LIB) $(CHECK_LIB) $(LDFLAGS)

# Правило сборки объектников из tests/*.c
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Запуск всех тестов
test: $(TEST_EXECUTABLES)
	@echo "Running tests..."
	@for t in $(TEST_EXECUTABLES); do \
		echo "===== Running $$t ====="; \
		$$t || exit 1; \
	done
	@echo "All tests passed."

# Генерация отчёта по покрытию при помощи lcov + genhtml
coverage: test
	@echo "Generating coverage report..."
	@lcov --directory . --capture --output-file coverage.info 2>/dev/null
	@lcov --remove coverage.info '/usr/*' --output-file coverage.info 2>/dev/null
	@genhtml coverage.info --output-directory coverage 2>/dev/null
	@echo "Coverage report generated in ./coverage/index.html"

# Если предпочитаете gcovr:
# coverage:
# 	make test
# 	gcovr -r . --html --html-details -o coverage.html

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) coverage *.info *.gcda *.gcno

# Вывод подсказки
help:
	@echo "Makefile for Racecourse Management System"
	@echo "Usage:"
	@echo "  make            - Build the project"
	@echo "  make test       - Build and run tests"
	@echo "  make coverage   - Generate coverage report (needs lcov & genhtml)"
	@echo "  make clean      - Clean up build artifacts"
	@echo "  make help       - Show this help message"

.PHONY: all clean help test coverage
