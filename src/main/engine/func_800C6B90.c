typedef signed short s16;

extern char *D_8009D254;

int func_800C6B90(s16 *pos, int extraRadius) {
    register char *entity_v1 asm("$3") = D_8009D254;
    register int x asm("$6");
    register int radius asm("$7");
    register int z asm("$3");
    register int pos_value asm("$2");
    register int x_sq asm("$8");
    register int z_sq asm("$2");
    register int radius_sq asm("$6");
    register int dist asm("$2");

    asm volatile(
        ".set push\n\t"
        ".set noreorder\n\t"
        "nop\n\t"
        "lh %0,42(%3)\n\t"
        "lh %1,548(%3)\n\t"
        "addiu $sp,$sp,-24\n\t"
        "sh %0,16($sp)\n\t"
        "lh $2,46(%3)\n\t"
        "nop\n\t"
        "sh $2,18($sp)\n\t"
        "lh %2,50(%3)\n\t"
        "nop\n\t"
        "sh %2,20($sp)\n\t"
        ".set pop"
        : "=r"(x), "=r"(radius), "=r"(z)
        : "r"(entity_v1)
        : "$2", "$sp", "memory");

    pos_value = pos[0];
    x = x - pos_value;
    asm volatile(
        "mult %0,%0\n\t"
        "sw %0,0($sp)"
        :
        : "r"(x)
        : "memory");

    pos_value = pos[2];
    asm volatile("mflo %0" : "=r"(x_sq));
    z = z - pos_value;
    asm volatile(
        "nop\n\t"
        "mult %0,%0"
        :
        : "r"(z));
    asm volatile("mflo %0" : "=r"(z_sq));
    radius = radius + extraRadius;
    asm volatile(
        "nop\n\t"
        "mult %0,%0"
        :
        : "r"(radius));
    asm volatile("sw %0,8($sp)" : : "r"(z) : "memory");
    dist = x_sq + z_sq;
    asm volatile("mflo %0" : "=r"(radius_sq));
    dist = dist < radius_sq;
    asm volatile("addiu $sp,$sp,24" ::: "$sp", "memory");
    return dist;
}
