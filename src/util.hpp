#pragma once

#include "types.hpp"
#include <gmp.h>

void random_oclmp(oclmp &n, size_t precision);
void random_oclmp_pool(oclmp_pool &ns, size_t precision);

void print_oclmp(const oclmp &num);

void oclmp_to_gmp(mpz_t& result, oclmp& mp);

void gmp_to_oclmp(mpz_t& value, oclmp& mp);
