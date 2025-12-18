#ifndef READERS_WRITERS_H
#define READERS_WRITERS_H

#include <shared_mutex>
#include <mutex>
#include <condition_variable>
#include <atomic>

// Читатели-писатели

// 1. Приоритет читателей
class ReadersPriority {
private:
    std::shared_mutex mtx;

public:
    void start_read() {
        mtx.lock_shared();
    }

    void end_read() {
        mtx.unlock_shared();
    }

    void start_write() {
        mtx.lock();
    }

    void end_write() {
        mtx.unlock();
    }
};

// 2. Приоритет писателей
class WritersPriority {
private:
    std::mutex mtx;
    std::condition_variable read_cv;
    std::condition_variable write_cv;

    int active_readers = 0;
    int waiting_writers = 0;
    bool active_writer = false;

public:
    void start_read() {
        std::unique_lock<std::mutex> lock(mtx);
        read_cv.wait(lock, [this]() {
            return !active_writer && waiting_writers == 0;
        });
        active_readers++;
    }

    void end_read() {
        std::unique_lock<std::mutex> lock(mtx);
        active_readers--;
        if (active_readers == 0) {
            write_cv.notify_one();
        }
    }

    void start_write() {
        std::unique_lock<std::mutex> lock(mtx);
        waiting_writers++;
        write_cv.wait(lock, [this]() {
            return active_readers == 0 && !active_writer;
        });
        waiting_writers--;
        active_writer = true;
    }

    void end_write() {
        std::unique_lock<std::mutex> lock(mtx);
        active_writer = false;
        if (waiting_writers > 0) {
            write_cv.notify_one();
        } else {
            read_cv.notify_all();
        }
    }
};

// 3. Справедливая версия
class FairReadersWriters {
private:
    std::mutex mtx;
    std::condition_variable cv;

    int active_readers = 0;
    bool active_writer = false;
    int waiting_operations = 0;

public:
    void start_read() {
        std::unique_lock<std::mutex> lock(mtx);
        waiting_operations++;
        cv.wait(lock, [this]() {
            return !active_writer;
        });
        waiting_operations--;
        active_readers++;
    }

    void end_read() {
        std::unique_lock<std::mutex> lock(mtx);
        active_readers--;
        if (active_readers == 0) {
            cv.notify_all();
        }
    }

    void start_write() {
        std::unique_lock<std::mutex> lock(mtx);
        waiting_operations++;
        cv.wait(lock, [this]() {
            return active_readers == 0 && !active_writer;
        });
        waiting_operations--;
        active_writer = true;
    }

    void end_write() {
        std::unique_lock<std::mutex> lock(mtx);
        active_writer = false;
        cv.notify_all();
    }
};

#endif
