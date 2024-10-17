typedef unsigned char u32;

__kernel void oclmp_bitwise_or(const int n, __global const u32* A, __global const u32* B, __global u32* C) {
    int id = get_global_id(0);
    C[id] = A[id] & B[id];
}

__kernel void oclmp_bitwise_and(const int n, __global const u32* A, __global const u32* B, __global u32* C) {
    int id = get_global_id(0);
    C[id] = A[id] & B[id];
}

__kernel void oclmp_bitwise_xor(const int n, __global const u32* A, __global const u32* B, __global u32* C) {
    int id = get_global_id(0);
    C[id] = A[id] ^ B[id];
}
