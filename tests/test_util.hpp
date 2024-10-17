#pragma once

#include "types.hpp"
#include <gtest/gtest.h>

inline bool oclmp_equal(oclmp_t x, oclmp_t e) {
    if (x.size != e.size)
        return false;

    for (int i = 0; i < e.size; i++) {
        if (x.data[i] != e.data[i])
            return false;
    }

    return true;
}

#define EXPECT_OCLMP_EQ(x, e) \
    EXPECT_TRUE(oclmp_equal(x, e))
