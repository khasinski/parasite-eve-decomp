typedef signed short s16;

int func_800C6148(char *obj, s16 *pos, unsigned short radius) {
    int x = *(s16 *)(obj + 0x2A) - pos[0];
    register int obj_z asm("$2");
    int pos_z;
    int z;
    register int x_sq asm("$7");
    int z_sq;
    int dist;
    register int radius_sq asm("$9");
    int radius_u;
    asm volatile(
        "mult %0,%0\n\t"
        "addiu $sp,$sp,-16\n\t"
        "sw %0,0($sp)"
        :
        : "r"(x)
        : "$sp", "memory");

    obj_z = *(s16 *)(obj + 0x32);
    pos_z = pos[2];
    asm volatile("mflo %0" : "=r"(x_sq));
    z = obj_z - pos_z;
    asm volatile(
        "nop\n\t"
        "mult %0,%0"
        :
        : "r"(z));
    asm volatile("mflo %0" : "=r"(z_sq));
    radius_u = radius;
    asm volatile(
        "nop\n\t"
        "mult %0,%0"
        :
        : "r"(radius_u));
    asm volatile("sw %0,8($sp)" : : "r"(z) : "memory");
    dist = x_sq + z_sq;
    asm volatile("mflo %0" : "=r"(radius_sq));
    dist = dist < radius_sq;
    asm volatile("addiu $sp,$sp,16" ::: "$sp", "memory");
    return dist;
}
