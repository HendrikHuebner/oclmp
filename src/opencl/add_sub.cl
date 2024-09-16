typedef unsigned char b256int_t;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

__kernel void oclmp_add_savec(__global const b256int_t* A, __global const b256int_t* B, 
                        __global b256int_t* C,  __global unsigned int* S) {
    int id = get_global_id(0);
    int idx = id << 2;
    u32 a = ((u32*) A)[idx];
    u32 b = ((u32*) B)[idx];
    u64 l = (u64)a + (u64)b;
    ((u32*) S)[id] = (u32) (l >> 32);
    ((u32*) C)[idx] = (u32) l;
}

__kernel void oclmp_add_n(const int n, __global const b256int_t** A, __global b256int_t* C,  __global u32* S) {
    int id = get_global_id(0);
    int idx = id << 2;

    u64 c = 0;
    for (int i = 0; i < n; i++) {
        u32 a = ((u32*) A[i])[idx];
        c = (u64)a + (u64)c;
    }

    ((u32*) S)[id] = (u32) (l >> 32);
    ((u32*) C)[idx] = (u32) (l & 0xFFFFFFFF);
}

__kernel void oclmp_combine(const int n, __global const b256int_t* A,  __global const u32* S, __global b256int_t** C) {
    u32 long carry = 0;
    ((u32*) C)[0] = ((u32*) A)[0];

    for (int i = 1; i < n; i++) {
        u64 c = ((u32*) A)[i] + ((u32*) S)[i];
        ((u32*) C)[i] = (u32) (c & 0xFFFFFFFF);
        carry = (u32) (c >> 32);
    }

    ((u32*) C)[n] = carry;
}
