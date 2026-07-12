typedef signed short s16;

int func_800C6584(s16 *a, int radiusA, s16 *b, int radiusB) {
    int x = a[0] - b[0];
    register int obj_z asm("$2");
    register int pos_z asm("$3");
    register int z asm("$2");
    register int x_sq asm("$8");
    register int z_sq asm("$3");
    register int dist asm("$2");
    register int radius asm("$5");
    register int radius_sq asm("$10");

    asm volatile(
        "mult %0,%0\n\t"
        "addiu $sp,$sp,-16\n\t"
        "sw %0,0($sp)"
        :
        : "r"(x)
        : "$sp", "memory");

    obj_z = a[2];
    pos_z = b[2];
    asm volatile("mflo %0" : "=r"(x_sq));
    z = obj_z - pos_z;
    asm volatile(
        "nop\n\t"
        "mult %0,%0"
        :
        : "r"(z));
    asm volatile("mflo %0" : "=r"(z_sq));
    radius = radiusA + radiusB;
    asm volatile(
        "nop\n\t"
        "mult %0,%0"
        :
        : "r"(radius));
    asm volatile("sw %0,8($sp)" : : "r"(z) : "memory");
    dist = x_sq + z_sq;
    asm volatile("mflo %0" : "=r"(radius_sq));
    dist = dist < radius_sq;
    asm volatile("addiu $sp,$sp,16" ::: "$sp", "memory");
    return dist;
}
