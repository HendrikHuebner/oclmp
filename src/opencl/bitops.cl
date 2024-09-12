typedef unsigned char b256int_t;

__kernel void oclmp_bitwise_or(__global const b256int_t* A, __global const b256int_t* B, __global b256int_t* C) {
    int id = get_global_id(0);
    C[id] = A[id] | B[id];
}

__kernel void oclmp_and(__global const b256int_t* A, __global const b256int_t* B, __global b256int_t* C) {
    int id = get_global_id(0);
    C[id] = A[id] & B[id];
}

__kernel void oclmp_xor(__global const b256int_t* A, __global const b256int_t* B, __global b256int_t* C) {
    int id = get_global_id(0);
    C[id] = A[id] ^ B[id];
}
