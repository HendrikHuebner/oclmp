#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"
#include "test_util.hpp"

TEST(OCLMPTest, AddTest) {
    size_t prec = 1000;
    oclmp a = random_oclmp(prec);
    oclmp b = random_oclmp(prec);

    oclmp c, d;
    alloc_oclmp(prec + 4, c);
    alloc_oclmp(prec + 4, d);

    mpz_t gmp_a, gmp_b, gmp_c;
    mpz_init(gmp_a);
    mpz_init(gmp_b);
    mpz_init(gmp_c);

    oclmp_to_gmp(gmp_a, a);
    oclmp_to_gmp(gmp_b, b);

    mpz_add(gmp_c, gmp_a, gmp_b);
    
    gmp_to_oclmp(gmp_c, d);

    oclmp_env ctx("../src/opencl");

    load_oclmp(ctx, a);
    load_oclmp(ctx, b);
    load_oclmp(ctx, c);

    oclmp_add(ctx, a, b, c);

    fetch_oclmp(ctx, c);

    EXPECT_OCLMP_EQ(c, d);

    mpz_clear(gmp_a);
    mpz_clear(gmp_b);
    mpz_clear(gmp_c);

    clear_oclmp(ctx, a);
    clear_oclmp(ctx, b);
    clear_oclmp(ctx, c);
    clear_oclmp(ctx, d);
    ctx.close();
}
