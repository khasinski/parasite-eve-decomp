void func_80194D7C(void *arg0, char *arg1, char *arg2) {
    *(short *)(arg2 + 0x10) =
        *(unsigned short *)(arg2 + 0x10) + *(unsigned short *)(arg2 + 0x12);

    /* Keep arg2 in a2 so the stack-adjust branch delay slot matches. */
    asm volatile(
        "lhu $2,0x12($6)\n\t"
        "lh $3,0x14($6)\n\t"
        "addiu $2,$2,0xa\n\t"
        "sh $2,0x12($6)\n\t"
        "addu $2,$3,$0\n\t"
        "slti $3,$3,0x9\n\t"
        "bnez $3,1f\n\t"
        "addiu $sp,$sp,-8\n\t"
        "addiu $2,$2,-8\n\t"
        "sh $2,0x14($6)\n"
        "1:"
        :
        :
        : "$2", "$3", "memory");

    if (*(short *)(arg1 + 0x2) == 60) {
        arg1[1] = 2;
    }

    asm volatile("addiu $sp,$sp,8");
}
