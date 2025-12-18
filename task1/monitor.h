#ifndef MONITOR_H
#define MONITOR_H

#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>

// Monitor - mutex + condition_variable
class Monitor {
private:
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    template<typename Func>
    void synchronized(Func func) {
        std::unique_lock<std::mutex> lock(mtx);
        func();
    }

    template<typename Predicate>
    void wait(Predicate condition) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, condition);
    }

    template<typename Predicate, typename Func>
    void wait_and_execute(Predicate condition, Func func) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, condition);
        func();
    }

    void notify_one() {
        cv.notify_one();
    }

    void notify_all() {
        cv.notify_all();
    }

    std::unique_lock<std::mutex> get_lock() {
        return std::unique_lock<std::mutex>(mtx);
    }

    std::condition_variable& get_cv() {
        return cv;
    }
};

// Пример - Producer-Consumer
template<typename T>
class MonitoredQueue {
private:
    std::queue<T> queue;
    Monitor monitor;
    size_t max_size;

public:
    MonitoredQueue(size_t max = 100) : max_size(max) {}

    void push(const T& item) {
        auto lock = monitor.get_lock();
        monitor.get_cv().wait(lock, [this]() {
            return queue.size() < max_size;
        });
        queue.push(item);
        monitor.notify_one();
    }

    T pop() {
        auto lock = monitor.get_lock();
        monitor.get_cv().wait(lock, [this]() {
            return !queue.empty();
        });
        T item = queue.front();
        queue.pop();
        monitor.notify_one();
        return item;
    }

    size_t size() {
        auto lock = monitor.get_lock();
        return queue.size();
    }
};

#endif
