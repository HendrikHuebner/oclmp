#pragma once

#include <CL/cl.h>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct oclmp {
    size_t size;
    size_t int_size;
    size_t frac_size;
    cl_mem cl_buf = nullptr;
    u8 *data;
};

struct oclmp_pool {
    size_t size;  /* Size of each element */
    size_t count; /* Number of elements */
    cl_mem cl_buf = nullptr;
    oclmp *oclmps;
    u8 *data;

    oclmp& operator[](size_t i) {
        return oclmps[i];
    }
};

struct oclmp_data {
    int id;
    oclmp_pool *src;
};

struct oclmp_temp {
    int id;

    oclmp_temp() {
        static int idx = 0;
        id = idx++;
    }
};

int alloc_oclmp_pool(size_t size, oclmp_pool& n, size_t count);

int oclmp_pool_init(size_t size, oclmp_pool& pool, std::vector<std::string> strs);

int oclmp_pool_init(size_t size, oclmp_pool& pool, size_t count, unsigned int init);

void oclmp_pool_clear(oclmp_pool& pool);

void oclmp_set_source_pool(oclmp_data& n, oclmp_pool& pool);

void oclmp_set(oclmp &n, const std::string str);

void oclmp_set(oclmp &n, const std::vector<uint8_t>& bytes);

void oclmp_set(oclmp &n, uint32_t i);

void oclmp_set(oclmp &n, uint64_t i);
