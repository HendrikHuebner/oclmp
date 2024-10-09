#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"
#include "test_util.hpp"
#include <omp.h>

TEST(MulTest, MulTest) {
    const std::vector<uint8_t> num2 = {0xff, 0x12, 0x8, 0x10};
    const std::vector<uint8_t> num1 = {0x80, 0x10};
    size_t prec = 8;
    oclmp a =  random_oclmp(prec);
    oclmp b =  random_oclmp(prec);
    oclmp c, d;
    alloc_oclmp(a.size + b.size, c);
    alloc_oclmp(a.size + b.size, d);

    mpz_t gmp_a, gmp_b, gmp_c;
    mpz_init(gmp_a);
    mpz_init(gmp_b);
    mpz_init(gmp_c);

    oclmp_to_gmp(gmp_a, a);
    oclmp_to_gmp(gmp_b, b);

    mpz_mul(gmp_c, gmp_a, gmp_b);
 
    gmp_to_oclmp(gmp_c, d);

    oclmp_env ctx("/home/hhuebner/Documents/OCLMP/src/opencl");

    load_oclmp(ctx, a);
    load_oclmp(ctx, b);
    load_oclmp(ctx, c);

    oclmp_mul(ctx, a, b, c);
    oclmp_run(ctx);

    fetch_oclmp(ctx, c);

    print_oclmp(a);
    print_oclmp(b);
    print_oclmp(c);
    print_oclmp(d);
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
