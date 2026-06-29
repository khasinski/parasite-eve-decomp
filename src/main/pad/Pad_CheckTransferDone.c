extern int *D_8009B4B4;

int Pad_CheckTransferDone(void) {
    int ret;

    asm volatile(
        ".set\tnoat\n\t"
        ".set\tnoreorder\n\t"
        "lui\t$3,%%hi(D_8009B4B4)\n\t"
        "lw\t$3,%%lo(D_8009B4B4)($3)\n\t"
        "nop\n\t"
        "lw\t$2,4($3)\n\t"
        "nop\n\t"
        "andi\t$2,$2,1\n\t"
        "beqz\t$2,1f\n\t"
        "addu\t$2,$zero,$zero\n\t"
        "lw\t$2,0($3)\n\t"
        "nop\n\t"
        "andi\t$2,$2,1\n\t"
        "bnez\t$2,1f\n\t"
        "addiu\t$2,$zero,1\n\t"
        "addu\t$2,$zero,$zero\n"
        "1:\n\t"
        ".set\treorder\n\t"
        ".set\tat"
        : "=r"(ret)
        :
        : "$3", "memory");

    return ret;
}
