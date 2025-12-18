#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include "readers_writers.h"

int shared_data = 0;
std::atomic<int> read_count{0};
std::atomic<int> write_count{0};
std::atomic<int> reader_wait_time{0};
std::atomic<int> writer_wait_time{0};

// Читатель
template<typename RW>
void reader(RW& rw, int id, int iterations) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sleep_dist(10, 50);

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        rw.start_read();
        auto end = std::chrono::steady_clock::now();
        auto wait = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        reader_wait_time += wait;

        int value = shared_data;
        read_count++;
        std::cout << "[R" << id << "] Прочитано: " << value << std::endl;

        rw.end_read();

        // Работа вне критической секции
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_dist(gen)));
    }
}

// Писатель
template<typename RW>
void writer(RW& rw, int id, int iterations) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sleep_dist(10, 50);

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::steady_clock::now();

        rw.start_write();
        auto end = std::chrono::steady_clock::now();
        auto wait = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        writer_wait_time += wait;

        shared_data++;
        write_count++;
        std::cout << "[W" << id << "] Записано: " << shared_data << std::endl;

        rw.end_write();

        // Работа вне критической секции
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_dist(gen)));
    }
}

template<typename RW>
void testReadersWriters(const std::string& name, int num_readers, int num_writers, int iterations) {
    std::cout << "\n=== " << name << " ===" << std::endl;

    // Сброс счетчиков
    shared_data = 0;
    read_count = 0;
    write_count = 0;
    reader_wait_time = 0;
    writer_wait_time = 0;

    RW rw;
    std::vector<std::thread> threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Запуск читателей
    for (int i = 0; i < num_readers; ++i) {
        threads.emplace_back(reader<RW>, std::ref(rw), i + 1, iterations);
    }

    // Запуск писателей
    for (int i = 0; i < num_writers; ++i) {
        threads.emplace_back(writer<RW>, std::ref(rw), i + 1, iterations);
    }

    // Ожидание завершения
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n--- Статистика ---" << std::endl;
    std::cout << "Общее время: " << total_time.count() << " мс" << std::endl;
    std::cout << "Всего чтений: " << read_count << std::endl;
    std::cout << "Всего записей: " << write_count << std::endl;
    std::cout << "Финальное значение: " << shared_data << std::endl;

    if (read_count > 0) {
        std::cout << "Среднее время ожидания читателя: "
                  << (reader_wait_time / read_count) << " мкс" << std::endl;
    }

    if (write_count > 0) {
        std::cout << "Среднее время ожидания писателя: "
                  << (writer_wait_time / write_count) << " мкс" << std::endl;
    }

    if (shared_data == num_writers * iterations) {
        std::cout << "Корректность: OK" << std::endl;
    } else {
        std::cout << "ОШИБКА: Ожидалось " << (num_writers * iterations)
                  << ", получено " << shared_data << std::endl;
    }
}

void runReadersWritersDemo() {
    const int NUM_READERS = 5;
    const int NUM_WRITERS = 2;
    const int ITERATIONS = 5;

    std::cout << "\n=== Задание 3: Читатели-писатели ===\n" << std::endl;

    std::cout << "Параметры:" << std::endl;
    std::cout << "  Читатели: " << NUM_READERS << std::endl;
    std::cout << "  Писатели: " << NUM_WRITERS << std::endl;
    std::cout << "  Итераций на поток: " << ITERATIONS << std::endl;

    testReadersWriters<ReadersPriority>(
        "1. Приоритет читателей",
        NUM_READERS, NUM_WRITERS, ITERATIONS
    );

    std::this_thread::sleep_for(std::chrono::seconds(1));

    testReadersWriters<WritersPriority>(
        "2. Приоритет писателей",
        NUM_READERS, NUM_WRITERS, ITERATIONS
    );

    std::this_thread::sleep_for(std::chrono::seconds(1));

    testReadersWriters<FairReadersWriters>(
        "3. Справедливая версия",
        NUM_READERS, NUM_WRITERS, ITERATIONS
    );

    std::cout << "\n=== Готово ===\n" << std::endl;
}
