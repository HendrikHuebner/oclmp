#pragma once

#include "types.hpp"
#include <gmp.h>

void random_oclmp(oclmp_t&n, size_t precision);
void random_oclmp_pool(oclmp_pool &ns, size_t precision);

void print_oclmp(const oclmp_t&num);

void oclmp_to_gmp(mpz_t& result, oclmp_t &mp);

void gmp_to_oclmp(mpz_t& value, oclmp_t &mp);
