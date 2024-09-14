typedef unsigned char b256int_t;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;

__kernel void oclmp_mul(__global const b256int_t* A, __global const b256int_t* B, 
                        __global b256int_t* C) {
    int id = get_global_id(0);
    u32 b = ((u32*) B)[idx];

    u64 sum = 0; 
    for (int i = 0; i <= id; i++) {
        int idx = i << 2;

        u16 a = ((u16*) A)[idx];
        u32 p = (u32)a * (u32)b;
        sum += p;
    }

    ((u32*) S)[id] = (p >> 32);
    ((u32*) C)[idx] = (u32) l;
}
