
#pragma once

#include "types.hpp"
#include <cstddef>
#include <cstdio>
#include <random>

oclmp_t random_oclmp(size_t precision) {
    b256int_t* array = new b256int_t[precision];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);

    for (size_t i = 0; i < precision; ++i) {
        array[i] = static_cast<unsigned char>(distrib(gen));
    }

    return {
        .data = array,
        .int_size = precision,
        .frac_size = 0,
        .size = precision
    };
}

void print_oclmp_t(const oclmp_t &num) {
    printf("Integer part: ");
    for (ssize_t i = num.int_size - 1; i >= 0; i--) {
        printf("%02x ", num.data[i]);
    }
    printf("\tFractional part: ");
    for (ssize_t i = num.frac_size + num.int_size - 1; i >= num.int_size; i--) {
        printf("%02x ", num.data[i]);
    }
    printf("\n");
}
