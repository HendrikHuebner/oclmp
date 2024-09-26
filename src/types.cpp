#include "types.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>

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

int alloc_oclmp(size_t precision, oclmp& n) {
    u8* data = new u8[precision];

    if (data == nullptr)
        return -1;

    n.data = data;
    n.int_size = precision;
    n.frac_size = 0;
    n.size = precision;

    return 0;
}

int alloc_oclmp_pool(size_t size, size_t count, oclmp_pool& ns) {
    u8* data = new u8[size * count];

    if (data == nullptr)
        return -1;

    ns.count = count;
    ns.size = size;
    ns.data = data;

    return 0;
}


oclmp make_oclmp(size_t precision, std::string str) {    
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

    u8* data = new u8[precision]();

    // Convert integer part to base-256
    size_t int_base256_len = 1;
    for (size_t i = 0; i < int_len; i++) {
        multiply_by_10(data, int_base256_len);
        add_digit(data, int_base256_len, int_part[i]);
    }

    if (int_base256_len > precision) 
        throw new std::invalid_argument("Integer part exceeds precision");
    
    // Convert fractional part to base-256
    size_t max_frac_length = precision - int_base256_len;
    size_t frac_base256_len = 0;
    for (size_t i = 0; i < frac_len && frac_base256_len < max_frac_length; i++) {
        multiply_by_10(data + int_base256_len, frac_base256_len);
        add_digit(data + int_base256_len, frac_base256_len, frac_part[i]);
    }
    
    delete[] int_part;
    delete[] frac_part;
    
    return {
        .data = data,
        .int_size = int_base256_len,
        .frac_size = frac_base256_len,
        .size = precision
    };
}

oclmp make_oclmp(std::vector<uint8_t>& bytes) {
    u8* data = new u8[bytes.size()]();
    std::memcpy(data, bytes.data(), bytes.size());

    return {
        .data = data,
        .int_size = bytes.size(),
        .frac_size = 0,
        .size = bytes.size()
    };
}

oclmp make_oclmp(size_t precision, uint32_t i) {
    if (precision < 4)
        throw new std::invalid_argument("OCLMP size must be greater than 3 to hold uint32");
    
    u8* data = new u8[precision]();
    std::memset(data, 0, precision);
    ((uint32_t *) data)[0] = i;


    return {
        .data = data,
        .int_size = precision,
        .frac_size = 0,
        .size = precision
    };
}

oclmp make_oclmp(size_t precision, uint64_t i) {
    if (precision < 8)
        throw new std::invalid_argument("OCLMP size must be greater than 7 to hold uint64");
    
    u8* data = new u8[precision]();
    std::memset(data, 0, precision);
    ((uint64_t *) data)[0] = i;


    return {
        .data = data,
        .int_size = precision,
        .frac_size = 0,
        .size = precision
    };
}
