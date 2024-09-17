#pragma once

#include "types.hpp"
#include <gmp.h>

oclmp random_oclmp(size_t precision);

void print_oclmp(const oclmp &num);

void oclmpo_gmp(mpz_t& result, oclmp& mp);

void gmp_to_oclmp(mpz_t& value, oclmp& mp);
