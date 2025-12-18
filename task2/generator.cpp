#include "generator.h"
#include <random>

std::vector<Product> generateProducts(int size) {
    std::vector<Product> products;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> code_dist(100, 999);
    std::uniform_int_distribution<> qty_dist(1, 100);
    std::uniform_real_distribution<> price_dist(10.0, 1000.0);

    std::vector<std::string> prefixes = {"A", "B", "C", "D", "E"};

    for (int i = 0; i < size; ++i) {
        Product p;
        p.code = prefixes[code_dist(gen) % prefixes.size()] + std::to_string(code_dist(gen));
        p.quantity = qty_dist(gen);
        p.price = price_dist(gen);
        products.push_back(p);
    }

    return products;
}
