void *TransMatrix(int *m, int *v) {
    register int x asm("$8") = v[0];
    register int y asm("$9") = v[1];
    register int z asm("$10") = v[2];
    register int *ret asm("$2");

    m[5] = x;
    m[6] = y;
    m[7] = z;

    asm volatile("addu %0,%1,$0" : "=r"(ret) : "r"(m));
    return ret;
}
