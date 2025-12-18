CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -pthread
INCLUDES = -I. -Itask1 -Itask2 -Itask3

# Целевой исполняемый файл
TARGET = lab4_full

# Исходные файлы
TASK1_SOURCES = task1/race_demo.cpp task1/benchmark.cpp
TASK2_SOURCES = task2/generator.cpp task2/processor.cpp task2/main.cpp
TASK3_SOURCES = task3/readers_writers_demo.cpp
MAIN_SOURCE = main_all.cpp

# Объектные файлы
TASK1_OBJECTS = $(TASK1_SOURCES:.cpp=.o)
TASK2_OBJECTS = $(TASK2_SOURCES:.cpp=.o)
TASK3_OBJECTS = $(TASK3_SOURCES:.cpp=.o)
MAIN_OBJECT = $(MAIN_SOURCE:.cpp=.o)

ALL_OBJECTS = $(TASK1_OBJECTS) $(TASK2_OBJECTS) $(TASK3_OBJECTS) $(MAIN_OBJECT)

# Цели по умолчанию
all: $(TARGET)

# Сборка главного исполняемого файла
$(TARGET): $(ALL_OBJECTS)
	@echo "Линковка $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Сборка завершена: $(TARGET)"

# Компиляция объектных файлов
%.o: %.cpp
	@echo "Компиляция $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Отдельные цели для каждого задания (опционально)
task1: task1/race_demo.o task1/benchmark.o
	@echo "✓ Задание 1 скомпилировано"

task2: task2/generator.o task2/processor.o task2/main.o
	@echo "✓ Задание 2 скомпилировано"

task3: task3/readers_writers_demo.o
	@echo "✓ Задание 3 скомпилировано"

# Очистка
clean:
	@echo "Очистка объектных файлов и исполняемых файлов..."
	rm -f $(ALL_OBJECTS) $(TARGET)
	@echo "✓ Очистка завершена"

# Перекомпиляция
rebuild: clean all

# Запуск программы
run: $(TARGET)
	@echo "Запуск $(TARGET)..."
	./$(TARGET)

# Помощь
help:
	@echo "Доступные команды:"
	@echo "  make           - Собрать все задания"
	@echo "  make task1     - Собрать только Задание 1"
	@echo "  make task2     - Собрать только Задание 2"
	@echo "  make task3     - Собрать только Задание 3"
	@echo "  make clean     - Удалить объектные и исполняемые файлы"
	@echo "  make rebuild   - Пересобрать проект с нуля"
	@echo "  make run       - Собрать и запустить программу"
	@echo "  make help      - Показать эту справку"

.PHONY: all clean rebuild run help task1 task2 task3
