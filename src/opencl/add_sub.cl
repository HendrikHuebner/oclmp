typedef unsigned char       u8;
typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned short      u16;

__kernel void oclmp_add(const int n, __global const u8 const* A, __global const u8* B, 
                        __global u8* C) {

    int id = get_global_id(0);

    A = &A[n * id];
    B = &B[n * id];
    C = &C[(n + 4) * id];
    int v = id | (id << 8) | (id << 16) | (id << 24);

    u32 carry = 0;
    
    for (int i = 0; i < (n >> 2); i++) {
        u64 c = (u64)((u32*) A)[i] + ((u32*) B)[i] + carry;

        ((u32*) C)[i] = (u32) (c & 0xFFFFFFFF);
        carry = (u32) (c >> 32ll);
    }

    ((u32*) C)[n >> 2] = carry;
}

__kernel void oclmp_add_n(const int n, __global const u8* __global* A, __global u8* C,  __global u32* S) {
    int id = get_global_id(0);
    int idx = id << 2;

    u64 c = 0;
    for (int i = 0; i < n; i++) {
        u32 a = ((u32*) A[i])[idx];
        c = (u64)a + (u64)c;
    }

    ((u32*) S)[id] = (u32) (c >> 32);
    ((u32*) C)[idx] = (u32) (c & 0xFFFFFFFF);
}

__kernel void oclmp_combine(const int a, const int b, __global const u8* A,  __global const u32* S, __global u8* C) {
    u32 carry = 0;
    ((u32*) C)[0] = ((u32*) A)[0];
    
    int n = (a + b) / 4;
    for (int i = 1; i < n; i++) {
        u64 c = ((u32*) A)[i] + ((u32*) S)[i] + carry;
        ((u32*) C)[i] = (u32) (c & 0xFFFFFFFF);
        carry = (u32) (c >> 32);
    }

    ((u32*) C)[a] = carry;
}
