#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"

TEST(OCLMPTest, AddTest) {
    const char* num1_str = "123456789";
    const char* num2_str = "987655432";
    size_t prec = 4;
    oclmp_t a = parse_oclmp(num1_str, prec);
    oclmp_t b = parse_oclmp(num2_str, prec);
    oclmp_t c = alloc_oclmp(prec);

    mpz_t gmp_a, gmp_b, gmp_c;
    mpz_init(gmp_a);
    mpz_init(gmp_b);
    mpz_init(gmp_c);

    mpz_set_str(gmp_a, num1_str, 10);
    mpz_set_str(gmp_b, num2_str, 10);
    mpz_ior(gmp_c, gmp_a, gmp_b);
    
    gmp_printf ("gmp %Zd\n", gmp_c);
    oclmp_t d = alloc_oclmp(prec);
    gmp_to_oclmp(gmp_c, d);

    oclmp_env ctx("../src/opencl");
    load_oclmp(ctx.ocl_manager, a);
    load_oclmp(ctx.ocl_manager, b);
    load_oclmp(ctx.ocl_manager, c);

    oclmp_bitwise_or(ctx, a, b, c);
    print_oclmp_t(c);
    print_oclmp_t(d);

    mpz_clear(gmp_a);
    mpz_clear(gmp_b);
    mpz_clear(gmp_c);
}
