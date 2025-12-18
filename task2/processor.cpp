#include "processor.h"
#include <regex>
#include <thread>
#include <mutex>

static std::mutex result_mutex;

double processSingleThreaded(const std::vector<Product>& products, const std::string& pattern) {
    std::regex regex_pattern(pattern);
    double total_cost = 0.0;

    for (const auto& product : products) {
        if (std::regex_match(product.code, regex_pattern)) {
            total_cost += product.quantity * product.price;
        }
    }

    return total_cost;
}

static void processChunk(const std::vector<Product>& products,
                        int start, int end,
                        const std::string& pattern,
                        double& result) {
    std::regex regex_pattern(pattern);
    double local_cost = 0.0;

    for (int i = start; i < end; ++i) {
        if (std::regex_match(products[i].code, regex_pattern)) {
            local_cost += products[i].quantity * products[i].price;
        }
    }

    std::lock_guard<std::mutex> lock(result_mutex);
    result += local_cost;
}

double processMultiThreaded(const std::vector<Product>& products,
                           const std::string& pattern,
                           int num_threads) {
    double total_cost = 0.0;
    std::vector<std::thread> threads;

    int chunk_size = products.size() / num_threads;
    int remainder = products.size() % num_threads;

    int start = 0;
    for (int i = 0; i < num_threads; ++i) {
        int end = start + chunk_size + (i < remainder ? 1 : 0);
        threads.emplace_back(processChunk, std::cref(products), start, end,
                           std::cref(pattern), std::ref(total_cost));
        start = end;
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return total_cost;
}
