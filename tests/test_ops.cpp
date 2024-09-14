#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(OCLMPTest, AddTest) {
    const char* num1_str = "123456789";
    const char* num2_str = "987655432";
    size_t prec = 100;
    oclmp_t a = random_oclmp(prec); //parse_oclmp(num1_str, 4);
    oclmp_t b =  random_oclmp(prec); //parse_oclmp(num2_str, 4);
    oclmp_t c = alloc_oclmp(prec);

    print_oclmp_t(a);
    print_oclmp_t(b);

    oclmp_context ctx("./src/opencl");
    load_oclmp(ctx.ocl_manager, a);
    load_oclmp(ctx.ocl_manager, b);
    load_oclmp(ctx.ocl_manager, c);

    oclmp_bitwise_or(ctx, a, b, c);
    print_oclmp_t(c);
}
