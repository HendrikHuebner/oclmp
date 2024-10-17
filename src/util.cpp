#include "util.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <random>

void random_oclmp(oclmp_t&n, size_t precision) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    
    precision = std::min(precision, n.size);

    for (size_t i = 0; i < precision; i++) {
        n.data[i] = static_cast<u8>(distrib(gen));
    }
}

void random_oclmp_pool(oclmp_pool &ns, size_t precision) {
    for (size_t i = 0; i < ns.count; i++) {
        random_oclmp(ns[i], precision);
    }
}

void print_oclmp(const oclmp_t&num) {
    printf("Integer part: ");
    for (ssize_t i = num.int_size - 1; i >= 0; i--) {
        printf("%02x ", num.data[i]);
    }
    /* printf("\tFractional part: ");
    for (ssize_t i = num.frac_size + num.int_size - 1; i >= num.int_size; i--) {
        printf("%02x ", num.data[i]);
    } */
    printf("\n");
}

void print_oclmp_pool(oclmp_pool &ns) {
    for (size_t i = 0; i < ns.size; i++) {
        print_oclmp(ns[i]);
    }
}

static void ucharArrayToMpz(mpz_t& result, unsigned char* array, size_t size) {
    mpz_import(result, size, -1, sizeof(unsigned char), 0, 0, array);
}

static void mpzToUcharArray(mpz_t& value, unsigned char* array, size_t size) {
    size_t count;
    mpz_export(array, &count, -1, sizeof(unsigned char), 0, 0, value);

    if (count < size) {
        std::memset(array + count, 0, size - count);
    }
}

void oclmp_to_gmp(mpz_t& result, oclmp_t &mp) {
    mpz_t int_part, frac_part;
    mpz_init(int_part);
    mpz_init(frac_part);
    
    ucharArrayToMpz(result, mp.data, mp.int_size);
    //ucharArrayToMpz(frac_part, mp.data + mp.int_size, mp.frac_size);
    //mpz_tdiv_q_2exp(frac_part, frac_part, mp.frac_size * 8);

    //mpz_add(result, int_part, frac_part);

    mpz_clear(int_part);
    mpz_clear(frac_part);
}

void gmp_to_oclmp(mpz_t& value, oclmp_t &mp) {
    mpz_t int_part, frac_part;
    mpz_init(int_part);
    mpz_init(frac_part);

    mpz_fdiv_q_2exp(int_part, value, mp.frac_size * 8);
    mpz_fdiv_r_2exp(frac_part, value, mp.frac_size * 8);

    mpzToUcharArray(int_part, mp.data, mp.int_size);

    mpz_mul_2exp(frac_part, frac_part, mp.frac_size * 8);
    mpzToUcharArray(frac_part, mp.data + mp.int_size, mp.frac_size);

    mpz_clear(int_part);
    mpz_clear(frac_part);
}
