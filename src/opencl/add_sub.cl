typedef unsigned char       u8;
typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned short      u16;

__kernel void oclmp_add(int n, int m, __global const u32* A, __global const u32* B, 
                        __global u32* C) {

    int id = get_global_id(0);
    u32 nMin = n < m ? n : m;

    A = &A[n * id];
    B = &B[n * id];
    C = &C[m * id];

    u32 carry = 0;
    int i = 0;
    
    for (; i < nMin; i++) {
        u64 c = (u64) A[i] + B[i] + carry;

        C[i] = (u32) (c & 0xFFFFFFFF);
        carry = (u32) (c >> 32ll);
    }

    if (n < m) 
        C[n] = carry;

    for (i = n + 1; i < m; i++)
        C[i] = 0;
}

__kernel void oclmp_reduce(const u32 count, const u32 N, __global const u32* pool, __global const u32* result) {
    int id = get_global_id(0);
    int lid = get_local_id(0);
    int lsize = get_local_size(0);

    __local u32 C[lsize];
    __local u32 S[lsize];

    u64 c = 0;
    for (int i = id; i < count; i += N) {
        c += pool[i];
    }

    C[lid] = (u32)c;
    S[lid] = (u32)(c >> 32);

    barrier(CLK_LOCAL_MEM_FENCE);

    u64 accumulator = 0;
    if (lid == 0) {
        for (int i = 0; i < lsize; i++) {
            accumulator += c[i];
        }
    }

    result[]

}

/*
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
*/
