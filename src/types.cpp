#include "types.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>

using size_t = std::size_t;

static void multiply_by_10(u8* result, size_t& len) {
    unsigned short carry = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned short prod = result[i] * 10 + carry;
        result[i] = prod % 256;
        carry = prod / 256;
    }
    if (carry > 0 && len < 256) {
        result[len] = carry;
        len++;
    }
}

static void add_digit(u8* result, size_t& len, u8 digit) {
    unsigned short carry = digit;
    for (size_t i = 0; i < len; i++) {
        unsigned short sum = result[i] + carry;
        result[i] = sum % 256;
        carry = sum / 256;
        if (carry == 0)
            break;
    }
    if (carry > 0 && len < 256) {
        result[len] = carry;
        len++;
    }
}

int alloc_oclmp_pool(size_t size, size_t count, oclmp_pool& ns) {
    u8* data = new u8[size * count]();

    if (data == nullptr)
        return -1;

    oclmp* oclmps = new oclmp[count];

    for (int i = 0; i < count; i++) {
        oclmps[i].data = &data[size * i];
        oclmps[i].size = size;
        oclmps[i].int_size = size;
        oclmps[i].frac_size = 0;
    }

    ns.count = count;
    ns.size = size;
    ns.data = data;
    ns.oclmps = oclmps;

    return 0;
}

int oclmp_pool_init(size_t size, oclmp_pool& pool, std::vector<std::string> strs) {
    int err = alloc_oclmp_pool(size, strs.size(), pool);
    
    if (err) 
        return err;

    for (int i = 0; i < pool.count; i++) {
        oclmp_set(pool[i], strs[i]);
    }

    return 0;
}

int oclmp_pool_init(size_t size, oclmp_pool& pool, size_t count, unsigned int init) {
    int err = alloc_oclmp_pool(size, count, pool);
    
    if (err) 
        return err;

    for (int i = 0; i < pool.count; i++) {
        oclmp_set(pool[i], init);
    }

    return 0;
}

void oclmp_pool_clear(oclmp_pool& pool) {
    delete[] pool.data;
    delete[] pool.oclmps;
}

void oclmp_set_source_pool(oclmp_data& n, oclmp_pool& pool) {
    static int id = 0;
    n.id = id++;
    n.src = &pool;
}

void oclmp_set(oclmp &n, std::string str) { 
    if (n.size % 4 != 0) throw std::invalid_argument("Size must be multiple of 4 (for now)");

    size_t b10_decimal_point = std::string::npos;
    u8* int_part = new u8[str.length()];
    u8* frac_part = new u8[str.length()];
    size_t int_len = 0, frac_len = 0;
    
    // Parse the integer and fractional parts
    size_t i = 0;
    for (; i < str.length(); i++) {
        if (str[i] == '.')
            break;
        if (str[i] < '0' || str[i] > '9')
            throw std::invalid_argument("Invalid character in integer part");

        int_part[int_len++] = str[i] - '0';
    }
    
    if (i < str.length() && str[i] == '.') {
        b10_decimal_point = i;

        i++;
        for (; i < str.length(); i++) {
            if (str[i] < '0' || str[i] > '9')
                throw std::invalid_argument("Invalid character in fractional part");
            frac_part[frac_len++] = str[i] - '0';
        }
    }

    // Convert integer part to base-256
    size_t int_base256_len = 1;
    for (size_t i = 0; i < int_len; i++) {
        multiply_by_10(n.data, int_base256_len);
        add_digit(n.data, int_base256_len, int_part[i]);
    }

    if (int_base256_len > n.size) 
        throw new std::invalid_argument("Integer part exceeds precision");
    
    // Convert fractional part to base-256
    size_t max_frac_length = n.size - int_base256_len;
    size_t frac_base256_len = 0;
    for (size_t i = 0; i < frac_len && frac_base256_len < max_frac_length; i++) {
        multiply_by_10(n.data + int_base256_len, frac_base256_len);
        add_digit(n.data + int_base256_len, frac_base256_len, frac_part[i]);
    }
    
    delete[] int_part;
    delete[] frac_part;
}

void oclmp_set(oclmp &n, const std::vector<uint8_t>& bytes) {
    if (n.size % 4 != 0) throw std::invalid_argument("Size must be multiple of 4 (for now)");
    if (bytes.size() > n.size) throw std::invalid_argument("Vector size does not match oclmp size");

    std::memcpy(n.data, bytes.data(), bytes.size());

}

void oclmp_set(oclmp &n, uint32_t i) {
    if (n.size < 4)
        throw new std::invalid_argument("OCLMP size must be greater than 3 to hold uint32");
    
    std::memset(n.data, 0, n.size);
    ((uint32_t *) n.data)[0] = i;
}

void oclmp_set(oclmp &n, uint64_t i) {
    size_t precision = n.size;
    if (precision % 4 != 0) throw std::invalid_argument("Size must be multiple of 4 (for now)");

    if (precision < 8)
        throw new std::invalid_argument("OCLMP size must be greater than 7 to hold uint64");
    
    std::memset(n.data, 0, precision);
    ((uint64_t *) n.data)[0] = i;
}
