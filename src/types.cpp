#include "types.hpp"
#include <stdexcept>

static void multiply_by_10(b256int_t* result, size_t& len) {
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

static void add_digit(b256int_t* result, size_t& len, b256int_t digit) {
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

oclmp alloc_oclmp(size_t precision) {
    b256int_t* data = new b256int_t[precision];
    return {
        .data = data,
        .int_size = precision,
        .frac_size = 0,
        .size = precision
    };
}

oclmp parse_oclmp(std::string str, size_t precision) {    
    size_t b10_decimal_point = std::string::npos;
    b256int_t* int_part = new b256int_t[str.length()];
    b256int_t* frac_part = new b256int_t[str.length()];
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

    b256int_t* data = new b256int_t[precision]();

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

oclmp parse_oclmp(std::vector<unsigned char>& bytes) {    
    return {
        .data = bytes.data(),
        .int_size = bytes.size(),
        .frac_size = 0,
        .size = bytes.size()
    };
}
