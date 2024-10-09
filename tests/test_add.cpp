#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"

TEST(OCLMPTest, AddTest) {
    const size_t size = 30;
    const size_t count = 10;

    oclmp_pool a, b, c;
    oclmp_pool_init(size, a, count, 0);
    oclmp_pool_init(size, b, count, 0);
    random_oclmp_pool(a, size);
    random_oclmp_pool(b, size);

    mpz_t gmp_a[count], gmp_b[count], gmp_c[count];

    for (int i = 0; i < count; i++) {
        mpz_init(gmp_a[i]);
        mpz_init(gmp_b[i]);
        mpz_init(gmp_c[i]);
        
        oclmp_to_gmp(gmp_a[i], a[i]);
        oclmp_to_gmp(gmp_b[i], b[i]);
        
        mpz_add(gmp_c[i], gmp_a[i], gmp_b[i]);

        gmp_printf("%d: %Zd \n", i, gmp_c[i]);
    }

    oclmp_env ctx("/home/hhuebner/Documents/OCLMP/src/opencl");

    oclmp_begin(ctx, count);
    oclmp_data A, B, C;
    oclmp_set_source_pool(A, a);
    oclmp_set_source_pool(B, b);
    oclmp_set_source_pool(C, c);

    oclmp_add(ctx, A, B, C);

    oclmp_run(ctx);

    ctx.close();
}
