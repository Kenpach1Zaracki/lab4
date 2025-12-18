#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include "spinlock.h"
#include "monitor.h"

// Демонстрация гонки потоков

int global_counter = 0;

char getRandomChar() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static thread_local std::uniform_int_distribution<> dis(33, 126);
    return static_cast<char>(dis(gen));
}

// Без синхронизации
void raceWithoutSync(int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        char ch = getRandomChar();
        std::cout << "[Thread " << thread_id << "] " << ch << " ";
        global_counter++;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

// С mutex
void raceWithMutex(int thread_id, int iterations, std::mutex& mtx) {
    for (int i = 0; i < iterations; ++i) {
        char ch = getRandomChar();
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "[Thread " << thread_id << "] " << ch << " ";
            global_counter++;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

// С spinlock
void raceWithSpinLock(int thread_id, int iterations, SpinLock& spinlock) {
    for (int i = 0; i < iterations; ++i) {
        char ch = getRandomChar();

        spinlock.lock();
        std::cout << "[Thread " << thread_id << "] " << ch << " ";
        global_counter++;
        spinlock.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

// С spinwait
void raceWithSpinWait(int thread_id, int iterations, SpinWait& spinwait) {
    for (int i = 0; i < iterations; ++i) {
        char ch = getRandomChar();

        spinwait.lock();
        std::cout << "[Thread " << thread_id << "] " << ch << " ";
        global_counter++;
        spinwait.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

// С monitor
void raceWithMonitor(int thread_id, int iterations, Monitor& monitor) {
    for (int i = 0; i < iterations; ++i) {
        char ch = getRandomChar();

        monitor.synchronized([&]() {
            std::cout << "[Thread " << thread_id << "] " << ch << " ";
            global_counter++;
        });

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void runRaceDemo() {
    const int NUM_THREADS = 5;
    const int ITERATIONS = 10;

    std::cout << "\n=== Задание 1: Демонстрация гонки ===\n" << std::endl;

    std::cout << "\n--- 1. Без синхронизации ---" << std::endl;

    global_counter = 0;
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(raceWithoutSync, i, ITERATIONS);
        }
        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout << "\n\nСчетчик: " << global_counter
              << " (ожидалось: " << NUM_THREADS * ITERATIONS << ")" << std::endl;
    if (global_counter != NUM_THREADS * ITERATIONS) {
        std::cout << "ГОНКА! Потеряно " << (NUM_THREADS * ITERATIONS - global_counter) << std::endl;
    }

    std::cout << "\n\n--- 2. С mutex ---" << std::endl;

    global_counter = 0;
    {
        std::mutex mtx;
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(raceWithMutex, i, ITERATIONS, std::ref(mtx));
        }
        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout << "\n\nСчетчик: " << global_counter
              << " (ожидалось: " << NUM_THREADS * ITERATIONS << ")" << std::endl;

    std::cout << "\n\n--- 3. С spinlock ---" << std::endl;

    global_counter = 0;
    {
        SpinLock spinlock;
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(raceWithSpinLock, i, ITERATIONS, std::ref(spinlock));
        }
        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout << "\n\nСчетчик: " << global_counter
              << " (ожидалось: " << NUM_THREADS * ITERATIONS << ")" << std::endl;

    std::cout << "\n\n--- 4. С spinwait ---" << std::endl;

    global_counter = 0;
    {
        SpinWait spinwait;
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(raceWithSpinWait, i, ITERATIONS, std::ref(spinwait));
        }
        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout << "\n\nСчетчик: " << global_counter
              << " (ожидалось: " << NUM_THREADS * ITERATIONS << ")" << std::endl;

    std::cout << "\n\n--- 5. С monitor ---" << std::endl;

    global_counter = 0;
    {
        Monitor monitor;
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(raceWithMonitor, i, ITERATIONS, std::ref(monitor));
        }
        for (auto& t : threads) {
            t.join();
        }
    }
    std::cout << "\n\nСчетчик: " << global_counter
              << " (ожидалось: " << NUM_THREADS * ITERATIONS << ")" << std::endl;

    std::cout << "\n=== Готово ===\n" << std::endl;
}
