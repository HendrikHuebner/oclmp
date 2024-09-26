typedef unsigned char u8;

__kernel void oclmp_bitwise_or(__global const u8* A, __global const u8* B, __global u8* C) {
    int id = get_global_id(0);
    C[id] = A[id] | B[id];
}

__kernel void oclmp_bitwise_and(__global const u8* A, __global const u8* B, __global u8* C) {
    int id = get_global_id(0);
    C[id] = A[id] & B[id];
}

__kernel void oclmp_bitwise_xor(__global const u8* A, __global const u8* B, __global u8* C) {
    int id = get_global_id(0);
    C[id] = A[id] ^ B[id];
}
