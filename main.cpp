#include <iostream>
#include <iomanip>
#include <chrono>
#include "product.h"
#include "generator.h"
#include "processor.h"

int main() {
    int array_size;
    int num_threads;
    std::string pattern;

    std::cout << "=== Лабораторная работа №4. Вариант 9 ===" << std::endl;
    std::cout << "Многопоточная обработка данных о товарах\n" << std::endl;

    std::cout << "Введите размер массива данных: ";
    std::cin >> array_size;

    std::cout << "Введите количество потоков: ";
    std::cin >> num_threads;

    std::cout << "Введите шаблон кода товара (регулярное выражение, например A.*): ";
    std::cin >> pattern;

    std::cout << "\nГенерация товаров..." << std::endl;
    auto products = generateProducts(array_size);

    std::cout << "\nПримеры сгенерированных товаров:" << std::endl;
    for (int i = 0; i < std::min(5, array_size); ++i) {
        std::cout << "  Код: " << products[i].code
                  << ", Количество: " << products[i].quantity
                  << ", Цена: " << std::fixed << std::setprecision(2)
                  << products[i].price << std::endl;
    }

    std::cout << "\n--- Однопоточная обработка ---" << std::endl;
    auto start_single = std::chrono::high_resolution_clock::now();
    double result_single = processSingleThreaded(products, pattern);
    auto end_single = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::microseconds>(
        end_single - start_single);

    std::cout << "Время обработки: " << duration_single.count() << " мкс" << std::endl;
    std::cout << "Общая стоимость товаров: " << std::fixed << std::setprecision(2)
              << result_single << std::endl;

    std::cout << "\n--- Многопоточная обработка (" << num_threads << " потоков) ---" << std::endl;
    auto start_multi = std::chrono::high_resolution_clock::now();
    double result_multi = processMultiThreaded(products, pattern, num_threads);
    auto end_multi = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::microseconds>(
        end_multi - start_multi);

    std::cout << "Время обработки: " << duration_multi.count() << " мкс" << std::endl;
    std::cout << "Общая стоимость товаров: " << std::fixed << std::setprecision(2)
              << result_multi << std::endl;

    std::cout << "\n--- Анализ производительности ---" << std::endl;
    double speedup = static_cast<double>(duration_single.count()) / duration_multi.count();
    std::cout << "Ускорение: " << std::fixed << std::setprecision(2)
              << speedup << "x" << std::endl;

    if (std::abs(result_single - result_multi) < 0.01) {
        std::cout << "Результаты совпадают ✓" << std::endl;
    } else {
        std::cout << "ОШИБКА: Результаты не совпадают!" << std::endl;
    }

    return 0;
}
