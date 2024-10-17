#pragma once

#include <CL/cl.h>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct oclmp_t{
    size_t size;
    size_t int_size;
    size_t frac_size;
    u8 *data;
};

struct oclmp_pool {
    size_t size;  /* Size of each element */
    size_t count; /* Number of elements */
    cl_mem cl_buf = nullptr;
    oclmp_t*oclmps;
    u8 *data;

    oclmp_t &operator[](size_t i) {
        return oclmps[i];
    }
};

static int operand_idx = 0;

struct oclmp_operand {
    const int id;
    const size_t size;
    cl_mem cl_buf;

  public:
    oclmp_operand(size_t size) : id(operand_idx++), size(size) {}
    virtual ~oclmp_operand() = default;

    bool operator==(const oclmp_operand& other) const {
        return this->id == other.id;
    }
};

struct oclmp_data : public oclmp_operand {
    oclmp_pool *src;

    oclmp_data(oclmp_pool *pool) : oclmp_operand(pool->size), src(pool) {
        this->cl_buf = pool->cl_buf;
    }
};

struct oclmp_temp : public oclmp_operand {
    oclmp_temp(size_t size) : oclmp_operand(size) {
        cl_buf = nullptr;
    }
};

namespace std {
    template <>
    struct hash<oclmp_operand> {
        std::size_t operator()(const oclmp_operand& operand) const {
            return std::hash<int>()(operand.id);
        }
    };
}

int alloc_oclmp_pool(size_t size, oclmp_pool& n, size_t count);

int oclmp_pool_init(size_t size, oclmp_pool& pool, const std::vector<std::string> &strs);

int oclmp_pool_init(size_t size, oclmp_pool& pool, const std::vector<std::vector<uint8_t>> &bytes);

int oclmp_pool_init(size_t size, oclmp_pool& pool, size_t count, unsigned int init);

void oclmp_pool_clear(oclmp_pool& pool);

void oclmp_set_source_pool(oclmp_data& n, oclmp_pool& pool);

void oclmp_set(oclmp_t&n, const std::string &str);

void oclmp_set(oclmp_t&n, const std::vector<uint8_t>& bytes);

void oclmp_set(oclmp_t&n, uint32_t i);

void oclmp_set(oclmp_t&n, uint64_t i);
