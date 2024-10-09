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
        oclmp o;
        alloc_oclmp(prec, o);
        result.push_back(o);
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
 
    std::cout << duration.count() << "us" << std::endl;

    // end of measurement

    for (int i = 0; i < n; i++) {
        fetch_oclmp(ctx, result[i]);
        clear_oclmp(ctx, values[i]);
    }


    ctx.close();
}
