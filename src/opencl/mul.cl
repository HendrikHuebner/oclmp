typedef unsigned char u8;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;


__kernel void oclmp_mul(__global const u16* A, __global const u16* B,
                        __global u32* C, __global u32* S) {
    int id = get_global_id(0);
    u64 sum = 0;
    
    for (int i = 0; i <= id; i++) {
        u16 b = ((u16*) B)[id];
        u16 a = ((u16*) A)[i];
        u32 p = (u32)a * (u32)b;
        sum += p;
    }

    ((u32*) S)[id] = (sum >> 32);
    ((u32*) C)[id] = (u32) sum;
    //printf("id %x, s %x, c %x \n", id, S[id], C[id]);
}

__kernel void oclmp_combine(const int n, __global const u32* A,  __global const u32* S, __global u16* C) {
    u64 carry = A[0] >> 16;
    C[0] = (u16) (A[0] & 0xFFFF);
    
    for (int i = 1; i < n; i++) {
        carry = A[i] + S[i - 1] + carry;
        //printf("O: %x %x %x \n", carry, A[i], S[i - 1]);
        C[i] = (u16) (carry & 0xFFFF);
        carry = carry >> 16;
    }

    C[n] = carry;
}


__kernel void base256_multiply(
    __global const uchar* A,        // First operand (array of base-256 digits)
    __global const uchar* B,        // Second operand (array of base-256 digits)
    __global uchar* result,         // Result array (twice the size of A and B)
    int N                           // Size of operands (A and B)
) {
    int global_id = get_global_id(0); // Unique thread ID (global)
    
    __local uint local_result[512];

    for (int i = global_id; i < 2 * N; i += get_global_size(0)) {
        local_result[i] = 0;
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = global_id; i < N; i += get_global_size(0)) {
        for (int j = 0; j < N; ++j) {
            uint product = (uint)A[i] * (uint)B[j];
            atomic_add(&local_result[i + j], product);
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = global_id; i < 2 * N; i += get_global_size(0)) {
        uint carry = 0;
        uint sum = local_result[i] + carry;
        result[i] = (uchar)(sum & 0xFF);  // Store least significant byte
        carry = sum >> 8;                 // Carry for next position
    }
}

__kernel void oclmp_addc(const int n, __global const u8* A, __global const u8* B,
                        __global const u8* S, __global u8* C) {
    u32 carry = 0;
    ((u32*) C)[0] = ((u32*) A)[0];

    for (int i = 1; i < n; i++) {
        u64 c = ((u32*) A)[i] + ((u32*) B)[i] + ((u32*) S)[i] + carry;
        ((u32*) C)[i] = (u32) (c & 0xFFFFFFFF);
        carry = (u32) (c >> 32);
    }

    ((u32*) C)[n] = carry;
}
