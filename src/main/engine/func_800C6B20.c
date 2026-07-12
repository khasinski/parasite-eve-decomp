typedef signed short s16;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

int func_800C62DC(SVECTOR *from, void *to);

extern char *D_8009D254;

int func_800C6B20(void *arg0) {
    SVECTOR pos;
    register char *entity_v1 asm("$3") = D_8009D254;
    register void *arg_s1 asm("$17") = arg0;
    register SVECTOR *pos_s0 asm("$16");
    register int first_v0 asm("$2");
    int first;

    asm volatile(
        ".set push\n\t"
        ".set noreorder\n\t"
        "lh $2,42(%2)\n\t"
        "addiu %0,$sp,16\n\t"
        "sh $2,16($sp)\n\t"
        "lh $2,46(%2)\n\t"
        "move $4,%0\n\t"
        "sh $2,18($sp)\n\t"
        "lh $2,50(%2)\n\t"
        "move $5,%3\n\t"
        ".word 0x0C000000\n\t"
        ".reloc .-4,R_MIPS_26,func_800C62DC\n\t"
        "sh $2,20($sp)\n\t"
        ".set pop"
        : "=r"(pos_s0), "=r"(first_v0)
        : "r"(entity_v1), "r"(arg_s1)
        : "$4", "$5", "$31", "memory");

    first = first_v0;
    return first | func_800C62DC(pos_s0, (char *)arg_s1 + 8);
}
