#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"

TEST(OCLMPTest, AddTest) {
    const size_t size = 128;
    const size_t count = 1000;

    oclmp_pool a, b, c;
    oclmp_pool_init(size, a, count, 0);
    oclmp_pool_init(size, b, count, 0);
    oclmp_pool_init(size + 4, c, count, 0);
    auto start = std::chrono::high_resolution_clock::now();

    random_oclmp_pool(a, size);
    random_oclmp_pool(b, size);

    mpz_t gmp_a[count], gmp_b[count], gmp_c[count];

    for (int i = 0; i < count; i++) {
        mpz_init(gmp_a[i]);
        mpz_init(gmp_b[i]);
        mpz_init(gmp_c[i]);
        
        oclmp_to_gmp(gmp_a[i], a[i]);
        oclmp_to_gmp(gmp_b[i], b[i]);
    }


    for (int i = 0; i < count; i++) {
        mpz_add(gmp_c[i], gmp_a[i], gmp_b[i]);
        mpz_add(gmp_c[i], gmp_c[i], gmp_b[i]);
        mpz_add(gmp_c[i], gmp_c[i], gmp_a[i]);

        //gmp_printf("%d: %Zx \n", i, gmp_c[i]);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << duration.count() << "us" << std::endl;

    oclmp_env ctx("/home/hhuebner/Documents/OCLMP/src/opencl");
    oclmp_load_pool(ctx, a);
    oclmp_load_pool(ctx, b);
    oclmp_load_pool(ctx, c);

    oclmp_begin(ctx, count);
    oclmp_data A, B, C;
    oclmp_set_source_pool(A, a);
    oclmp_set_source_pool(B, b);
    oclmp_set_source_pool(C, c);

    start = std::chrono::high_resolution_clock::now();

    oclmp_add(ctx, A, B, C);
    oclmp_add(ctx, C, B, C);
    oclmp_add(ctx, A, C, C);
    oclmp_run(ctx);

    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << duration.count() << "us" << std::endl;

    oclmp_fetch_pool(ctx, c);


    for (int i = 0; i < count; i++) {
        //print_oclmp(c[i]);
    }

    ctx.close();
}
