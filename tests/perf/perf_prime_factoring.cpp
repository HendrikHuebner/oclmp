#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"
#include "../test_util.hpp"

void pollards_rho_gmp(mpz_t n, mpz_t factor) {
    mpz_t x, y, d, c, tmp1, tmp2;
    mpz_inits(x, y, d, c, tmp1, tmp2, NULL);

    // x and y are initialized to 2, c to 1
    mpz_set_ui(x, 2);
    mpz_set_ui(y, 2);
    mpz_set_ui(c, 1);
    mpz_set_ui(d, 1);

    while (mpz_cmp_ui(d, 1) == 0) {
        // x = (x^2 + c) % n
        mpz_mul(tmp1, x, x); // tmp1 = x^2
        mpz_add(tmp1, tmp1, c); // tmp1 = x^2 + c
        mpz_mod(x, tmp1, n); // x = (x^2 + c) % n

        // y = (y^2 + c) % n twice for cycle detection
        mpz_mul(tmp1, y, y); // tmp1 = y^2
        mpz_add(tmp1, tmp1, c); // tmp1 = y^2 + c
        mpz_mod(y, tmp1, n); // y = (y^2 + c) % n

        mpz_mul(tmp1, y, y); // tmp1 = y^2
        mpz_add(tmp1, tmp1, c); // tmp1 = y^2 + c
        mpz_mod(y, tmp1, n); // y = (y^2 + c) % n

        // gcd(|x - y|, n)
        mpz_sub(tmp1, x, y); // tmp1 = x - y
        mpz_abs(tmp1, tmp1); // tmp1 = |x - y|
        mpz_gcd(d, tmp1, n); // d = gcd(|x - y|, n)
    }

    // If d is not equal to n, we have found a factor
    mpz_set(factor, d);
    mpz_clears(x, y, d, c, tmp1, tmp2, NULL);
}

void pollards_rho_oclmp(std::vector<std::string> &ns, std::vector<std::string> &factors) {
    int prec = 100;

    oclmp_pool inputs, ds;
    oclmp_pool_init(100, inputs, ns);
    oclmp_pool_init(100, ds, ns.size(), 1);

    oclmp_data n, d;
    oclmp_set_source_pool(n, inputs);
    oclmp_set_source_pool(d, ds);

    oclmp_temp x, y, c, tmp1, tmp2;

    // x and y are initialized to 2, c to 1
    oclmp_set_ui(x, 2);
    oclmp_set_ui(y, 2);
    oclmp_set_ui(c, 1);

    while (oclmp_cmp_ui(d, 1) == 0) {
        oclmp_begin(env);
        // x = (x^2 + c) % n
        oclmp_mul(tmp1, x, x); // tmp1 = x^2
        oclmp_add(tmp1, tmp1, c); // tmp1 = x^2 + c
        oclmp_mod(x, tmp1, n); // x = (x^2 + c) % n

        // y = (y^2 + c) % n twice for cycle detection
        oclmp_mul(tmp1, y, y); // tmp1 = y^2
        oclmp_add(tmp1, tmp1, c); // tmp1 = y^2 + c
        oclmp_mod(y, tmp1, n); // y = (y^2 + c) % n

        oclmp_mul(tmp1, y, y); // tmp1 = y^2
        oclmp_add(tmp1, tmp1, c); // tmp1 = y^2 + c
        oclmp_mod(y, tmp1, n); // y = (y^2 + c) % n

        // gcd(|x - y|, n)
        oclmp_sub(tmp1, x, y); // tmp1 = x - y
        oclmp_abs(tmp1, tmp1); // tmp1 = |x - y|
        oclmp_gcd(d, tmp1, n); // d = gcd(|x - y|, n)

        oclmp_run(env);
    }

    // If d is not equal to n, we have found a factor
    oclmp_export(factors, d);
    oclmp_clears(n, x, y, d, c, tmp1, tmp2);
}

TEST(PerformanceTest, AddTest) {
    const int n = 100;
    const int prec = 256;

    std::vector<oclmp> values;
    std::vector<oclmp> result;
    
    for (int i = 0; i < n; i++) {
        values.push_back(random_oclmp(prec));
        result.push_back(alloc_oclmp(prec));
    }

    oclmp_env ctx("../src/opencl");

    load_oclmp(ctx, values.data(), n);
    load_oclmp(ctx, result.data(), n);

    // start of measurement
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < n; i++) {
        oclmp_add(ctx, values[i], values[i], result[i]);
    }

    oclmp_run(ctx);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << duration.count() / 1000 << " ms" << std::endl;
 


    // end of measurement

    for (int i = 0; i < n; i++) {
        fetch_oclmp(ctx, result[i]);
        clear_oclmp(ctx, values[i]);
    }


    ctx.close();
}
