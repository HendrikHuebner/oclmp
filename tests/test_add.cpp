#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"
#include "test_util.hpp"

TEST(OCLMPTest, AddTest) {
    const char* num1_str = "123456789";
    const char* num2_str = "987655432";
    size_t prec = 4;
    oclmp a = parse_oclmp(num1_str, prec);
    oclmp b = parse_oclmp(num2_str, prec);
    oclmp c = alloc_oclmp(prec);

    mpz_t gmp_a, gmp_b, gmp_c;
    mpz_init(gmp_a);
    mpz_init(gmp_b);
    mpz_init(gmp_c);

    mpz_set_str(gmp_a, num1_str, 10);
    mpz_set_str(gmp_b, num2_str, 10);
    mpz_add(gmp_c, gmp_a, gmp_b);
    
    oclmp d = alloc_oclmp(prec);
    gmp_to_oclmp(gmp_c, d);

    oclmp_env ctx("../src/opencl");

    load_oclmp(ctx, a);
    load_oclmp(ctx, b);
    load_oclmp(ctx, c);
    
    print_oclmp(a);
    print_oclmp(b);

    oclmp_add(ctx, a, b, c);

    fetch_oclmp(ctx, c);

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
}
