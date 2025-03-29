# Makefile для проекта "Ипподром"

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -g
LDFLAGS = -lsqlite3 -lcrypto

# Исходные файлы
SRCS = main.cpp database.cpp auth_manager.cpp admin_controller.cpp \
       jockey_interface.cpp owner_interface.cpp race_manager.cpp reporting.cpp

# Объектные файлы
OBJS = $(SRCS:.cpp=.o)

# Исполняемый файл
TARGET = hippodrome

# Правило по умолчанию
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Правило для компиляции .cpp в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Заголовочные файлы как зависимости
database.o: database.h
auth_manager.o: auth_manager.h database.h
admin_controller.o: admin_controller.h database.h
jockey_interface.o: jockey_interface.h database.h
owner_interface.o: owner_interface.h database.h
race_manager.o: race_manager.h database.h
reporting.o: reporting.h database.h
main.o: auth_manager.h admin_controller.h jockey_interface.h owner_interface.h

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)

# Установка зависимостей (для Ubuntu/Debian)
install-deps:
	sudo apt-get install sqlite3 libsqlite3-dev libssl-dev

.PHONY: all clean install-deps