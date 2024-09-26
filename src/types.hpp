#pragma once

#include <CL/cl.h>
#include <cstddef>
#include <initializer_list>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

typedef unsigned char u8;

struct oclmp {
    u8* data;
    size_t int_size;
    size_t frac_size;
    size_t size;
    cl_mem cl_buf = nullptr;
};

struct oclmp_pool {
    u8* data;
    size_t size;  /* Size of each element */
    size_t count; /* Number of elements */
    cl_mem cl_buf = nullptr;

};

int alloc_oclmp(size_t precision, oclmp& n);

int alloc_oclmp_pool(size_t precision, oclmp_pool& ns);

template <typename... T>
inline int alloc_oclmp(size_t precision, oclmp& n, T... ns) {
    int err;

    if ((err = alloc_oclmp(precision, n)))
        return err;
    
    else
        return alloc_oclmp(precision, ns...);
}

inline int alloc_oclmp(size_t precision, std::vector<oclmp> &ns) {
    int err;

    for (oclmp n : ns)
        if ((err = alloc_oclmp(precision, n)))
            return err;
    
    return 0;
} 

oclmp make_oclmp(size_t precision, std::string str);

oclmp make_oclmp(std::vector<uint8_t>& bytes);

oclmp make_oclmp(size_t precision, uint64_t i);
