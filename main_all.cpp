#include <iostream>
#include <limits>
#include "task1/task1.h"
#include "task2/task2.h"
#include "task3/task3.h"

void printMainMenu() {
    std::cout << "\n=== Лабораторная работа #4: Многопоточность ===" << std::endl;
    std::cout << "Вариант 9 (Задание 2), Вариант 3 (Задание 3)" << std::endl;
    std::cout << "\nВыберите задание:" << std::endl;
    std::cout << "  1. Задание 1: Примитивы синхронизации - Демонстрация гонки" << std::endl;
    std::cout << "  2. Задание 1: Примитивы синхронизации - Бенчмарк" << std::endl;
    std::cout << "  3. Задание 2: Многопоточная обработка данных о товарах" << std::endl;
    std::cout << "  4. Задание 3: Читатели-писатели с выбором приоритета" << std::endl;
    std::cout << "  5. Запустить все задания последовательно" << std::endl;
    std::cout << "  0. Выход" << std::endl;
    std::cout << "\nВаш выбор: ";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void pauseBeforeContinue() {
    std::cout << "\n\nНажмите Enter для продолжения...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int main() {
    int choice;
    bool running = true;

    while (running) {
        printMainMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            clearInput();
            std::cout << "\nОшибка: введите число от 0 до 5\n" << std::endl;
            continue;
        }

        clearInput();

        switch (choice) {
            case 1:
                std::cout << "\n=== ЗАПУСК: Демонстрация гонки ASCII символов ===" << std::endl;
                runRaceDemo();
                pauseBeforeContinue();
                break;

            case 2:
                std::cout << "\n=== ЗАПУСК: Бенчмарк примитивов синхронизации ===" << std::endl;
                runBenchmark();
                pauseBeforeContinue();
                break;

            case 3:
                std::cout << "\n=== ЗАПУСК: Многопоточная обработка данных ===" << std::endl;
                runTask2();
                pauseBeforeContinue();
                break;

            case 4:
                std::cout << "\n=== ЗАПУСК: Читатели-писатели ===" << std::endl;
                runReadersWritersDemo();
                pauseBeforeContinue();
                break;

            case 5:
                std::cout << "\n=== ЗАПУСК ВСЕХ ЗАДАНИЙ ===" << std::endl;
                std::cout << "\n[1/4] Демонстрация гонки..." << std::endl;
                runRaceDemo();

                std::cout << "\n[2/4] Бенчмарк примитивов..." << std::endl;
                runBenchmark();

                std::cout << "\n[3/4] Пропускаем задание 2 (требует ввода)..." << std::endl;
                std::cout << "      Запустите его отдельно через меню (опция 3)\n" << std::endl;

                std::cout << "\n[4/4] Читатели-писатели..." << std::endl;
                runReadersWritersDemo();

                std::cout << "\n=== ВСЕ ЗАДАНИЯ ВЫПОЛНЕНЫ ===" << std::endl;
                pauseBeforeContinue();
                break;

            case 0:
                std::cout << "\nЗавершение работы программы..." << std::endl;
                running = false;
                break;

            default:
                std::cout << "\nОшибка: неверный выбор. Введите число от 0 до 5\n" << std::endl;
                break;
        }
    }

    std::cout << "\nСпасибо за использование программы!" << std::endl;
    std::cout << "Студент выполнил все задания лабораторной работы №4.\n" << std::endl;

    return 0;
}
