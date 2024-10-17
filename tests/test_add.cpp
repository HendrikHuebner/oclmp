#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"


TEST(OCLMPTest, AddTest) {
    const size_t size = 12;
    const size_t count = 4;

    std::vector<std::vector<uint8_t>> bytes = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        { 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    };

    oclmp_pool a, b, c;
    oclmp_pool_init(size, a, bytes);
    oclmp_pool_init(size, b, bytes);
    oclmp_pool_init(size + 4, c, count, 0);

    oclmp_env env("/home/hhuebner/Documents/OCLMP/src/opencl");
    oclmp_load_pool(env, a);
    oclmp_load_pool(env, b);
    oclmp_load_pool(env, c);

    oclmp_temp T1(size + 4), T2(size + 4);
    oclmp_data A(&a), B(&b), C(&c);
    
    oclmp_begin(env, count);

    oclmp_add(env, A, A, T2);
    oclmp_add(env, T2, T2, C);

    oclmp_run(env);
    oclmp_fetch_pool(env, c);

    //compare
    for (int i = 0; i < count; i++) {
        //print_oclmp(c[i]);
    }

    env.close();
}

TEST(OCLMPTest, AddTestRandom) {
    const size_t size = 200;
    const size_t count = 100000;

    oclmp_pool a, b, c;
    oclmp_pool_init(size, a, count, 0);
    oclmp_pool_init(size, b, count, 0);
    oclmp_pool_init(size + 4, c, count, 0);

    random_oclmp_pool(a, size);
    random_oclmp_pool(b, size);

    mpz_t gmp_a[count], gmp_b[count], gmp_c[count], gmp_d[count];


    for (int i = 0; i < count; i++) {
        mpz_init(gmp_a[i]);
        mpz_init(gmp_b[i]);
        mpz_init(gmp_c[i]);
        mpz_init(gmp_d[i]);
        
        oclmp_to_gmp(gmp_a[i], a[i]);
        oclmp_to_gmp(gmp_b[i], b[i]);
    }

    auto start = std::chrono::high_resolution_clock::now();

    # pragma omp parallel for
    for (int i = 0; i < count; i++) {
        mpz_add(gmp_c[i], gmp_a[i], gmp_a[i]);
        mpz_add(gmp_d[i], gmp_b[i], gmp_b[i]);
        mpz_add(gmp_c[i], gmp_c[i], gmp_d[i]);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << "MPZ add finished in: " << duration.count() << "us" << std::endl;

    oclmp_env env("/home/hhuebner/Documents/OCLMP/src/opencl");
    oclmp_load_pool(env, a);
    oclmp_load_pool(env, b);
    oclmp_load_pool(env, c);

    oclmp_temp T1(size + 4), T2(size + 4);

    oclmp_begin(env, count);
    oclmp_data A(&a), B(&b), C(&c);

    start = std::chrono::high_resolution_clock::now();

    oclmp_add(env, B, B, T1);
    oclmp_add(env, A, A, T2);
    oclmp_add(env, T1, T2, C);

    oclmp_run(env);

    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << "OCLMP add finished in: " << duration.count() << "us" << std::endl;

    oclmp_fetch_pool(env, c);

    //compare
    for (int i = 0; i < count; i++) {
        //print_oclmp(c[i]);
    }

    env.close();
}
