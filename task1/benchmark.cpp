#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <semaphore>
#include <barrier>
#include <iomanip>
#include "spinlock.h"
#include "monitor.h"

// Бенчмарк примитивов синхронизации

// Бенчмарк с mutex
template<typename Lock>
void benchmarkLock(Lock& lock, int thread_id, int iterations, long long& counter) {
    for (int i = 0; i < iterations; ++i) {
        lock.lock();
        counter++;
        lock.unlock();
    }
}

template<>
void benchmarkLock<std::mutex>(std::mutex& mtx, int thread_id, int iterations, long long& counter) {
    for (int i = 0; i < iterations; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        counter++;
    }
}

// Бенчмарк с semaphore
void benchmarkSemaphore(std::counting_semaphore<1>& sem, int thread_id, int iterations, long long& counter) {
    for (int i = 0; i < iterations; ++i) {
        sem.acquire();  // Захват (уменьшает счетчик)
        counter++;
        sem.release();  // Освобождение (увеличивает счетчик)
    }
}

// Бенчмарк с barrier
void benchmarkBarrier(std::barrier<>& barrier, int thread_id, int iterations, long long& counter) {
    for (int i = 0; i < iterations; ++i) {
        barrier.arrive_and_wait();
        if (thread_id == 0) {
            counter++;
        }
    }
}

// Бенчмарк с monitor
void benchmarkMonitor(Monitor& monitor, int thread_id, int iterations, long long& counter) {
    for (int i = 0; i < iterations; ++i) {
        monitor.synchronized([&]() {
            counter++;
        });
    }
}

template<typename Func, typename... Args>
double runBenchmark(const std::string& name, int num_threads, int iterations, Func func, Args&... args) {
    long long counter = 0;
    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(func, std::ref(args)..., i, iterations, std::ref(counter));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << std::setw(25) << std::left << name
              << "| Время: " << std::setw(10) << std::right << duration.count() << " мкс"
              << " | Счетчик: " << std::setw(10) << counter
              << " | Операций/сек: " << std::setw(12)
              << (counter * 1000000LL / duration.count())
              << std::endl;

    return static_cast<double>(duration.count());
}

void runBenchmark() {
    const int NUM_THREADS = 8;
    const int ITERATIONS = 100000;

    std::cout << "\n=== Бенчмарк примитивов ===\n" << std::endl;
    std::cout << "Потоков: " << NUM_THREADS << std::endl;
    std::cout << "Итераций: " << ITERATIONS << std::endl;
    std::cout << "Всего операций: " << NUM_THREADS * ITERATIONS << "\n" << std::endl;

    std::cout << std::string(90, '-') << std::endl;

    {
        std::mutex mtx;
        runBenchmark("std::mutex", NUM_THREADS, ITERATIONS, benchmarkLock<std::mutex>, mtx);
    }

    {
        std::counting_semaphore<1> sem(1);
        runBenchmark("std::semaphore", NUM_THREADS, ITERATIONS, benchmarkSemaphore, sem);
    }

    {
        std::barrier barrier(NUM_THREADS);
        runBenchmark("std::barrier", NUM_THREADS, ITERATIONS, benchmarkBarrier, barrier);
    }

    {
        SpinLock spinlock;
        runBenchmark("SpinLock", NUM_THREADS, ITERATIONS, benchmarkLock<SpinLock>, spinlock);
    }

    {
        SpinWait spinwait;
        runBenchmark("SpinWait", NUM_THREADS, ITERATIONS, benchmarkLock<SpinWait>, spinwait);
    }

    {
        Monitor monitor;
        runBenchmark("Monitor", NUM_THREADS, ITERATIONS, benchmarkMonitor, monitor);
    }

    std::cout << std::string(90, '-') << std::endl;
}
