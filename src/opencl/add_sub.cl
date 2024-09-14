typedef unsigned char b256int_t;

__kernel void oclmp_add_savec(__global const b256int_t* A, __global const b256int_t* B, 
                        __global b256int_t* C,  __global unsigned int* S) {
    int id = get_global_id(0);
    int idx = id << 2;
    unsigned int a = ((unsigned int*) A)[idx];
    unsigned int b = ((unsigned int*) B)[idx];
    unsigned long l = (unsigned long)a + (unsigned long)b;
    ((unsigned int*) S)[id] = (unsigned int) (l >> 32);
    ((unsigned int*) C)[idx] = (unsigned int) l;
}

__kernel void oclmp_add_n(const int n, __global const b256int_t** A, __global b256int_t* C,  __global unsigned int* S) {
    int id = get_global_id(0);
    int idx = id << 2;

    unsigned long c = 0;
    for (int i = 0; i < n; i++) {
        unsigned int a = ((unsigned int*) A[i])[idx];
        c = (unsigned long)a + (unsigned long)c;
    }

    ((unsigned int*) S)[id] = (unsigned int) (l >> 32);
    ((unsigned int*) C)[idx] = (unsigned int) l;
}
