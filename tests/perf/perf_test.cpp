#include <gmp.h>
#include <gtest/gtest.h>
#include "types.hpp"
#include "util.hpp"
#include "oclmp.hpp"
#include "../test_util.hpp"

TEST(PerformanceTest, AddTest) {
    const int n = 1;
    const int prec = 4;

    std::vector<oclmp> values;
    std::vector<oclmp> result;
    
    for (int i = 0; i < n; i++) {
        values.push_back(random_oclmp(prec));
        oclmp_to;
        alloc_oclmp(prec, o);
        result.push_back(o);
    }

    oclmp_env env("../src/opencl");

    load_oclmp(env, values.data(), n);
    load_oclmp(env, result.data(), n);

    // start of measurement
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < n; i++) {
        oclmp_add(env, values[i], values[i], result[i]);
    }

    oclmp_run(env);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
 
    std::cout << duration.count() << "us" << std::endl;

    // end of measurement

    for (int i = 0; i < n; i++) {
        fetch_oclmp(env, result[i]);
        clear_oclmp(env, values[i]);
    }


    env.close();
}
