#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"
#include "test_util.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(OCLMPTest, BitwiseOrTest) {
    const char* num1_str = "123456789";
    const char* num2_str = "987655432";
    size_t prec = 4;

    oclmp_ta = oclmp_set(prec, num1_str);
    oclmp_tb = oclmp_set(prec, num2_str);
    oclmp_tc, d;
    alloc_oclmp(prec, c);
    alloc_oclmp(prec, d);

    mpz_t gmp_a, gmp_b, gmp_c;
    mpz_init(gmp_a);
    mpz_init(gmp_b);
    mpz_init(gmp_c);
    mpz_set_str(gmp_a, num1_str, 10);
    mpz_set_str(gmp_b, num2_str, 10);
    mpz_ior(gmp_c, gmp_a, gmp_b);
        
    gmp_to_oclmp(gmp_c, d);

    oclmp_env env("../src/opencl");
    load_oclmp(env, a);
    load_oclmp(env, b);
    load_oclmp(env, c);
    oclmp_bitwise_or(env, a, b, c);

    fetch_oclmp(env, c);

    EXPECT_OCLMP_EQ(c, d);

    mpz_clear(gmp_a);
    mpz_clear(gmp_b);
    mpz_clear(gmp_c);

    clear_oclmp(env, a);
    clear_oclmp(env, b);
    clear_oclmp(env, c);
    clear_oclmp(env, d);
}
