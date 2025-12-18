#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>
#include "product.h"

double processSingleThreaded(const std::vector<Product>& products, const std::string& pattern);
double processMultiThreaded(const std::vector<Product>& products, const std::string& pattern, int num_threads);

#endif
