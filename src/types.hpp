#pragma once

#include <CL/cl.h>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

typedef unsigned char b256int_t;

struct oclmp {
    b256int_t* data;
    size_t int_size;
    size_t frac_size;
    size_t size;
    cl_mem cl_buf = nullptr;

};


oclmp alloc_oclmp(size_t precision);

oclmp parse_oclmp(std::string str, size_t precision);

oclmp parse_oclmp(std::vector<unsigned char>& bytes);

// Function to print the base-256 fixed-point number
void print_oclmp(const oclmp &num);
