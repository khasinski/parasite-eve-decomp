extern int EnterCriticalSection(void);
extern int ExitCriticalSection(void);
extern int SysDeqIntRP(int, void *);
extern int D_800A34B0;

extern short *D_8009B4B0;

extern int *D_8009B4B4;

int RemovePatchPad(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, &D_800A34B0);
    ExitCriticalSection();
    return 1;
}

void Pad_VBlankHandler(void) {
    asm volatile(
        ".set\tnoreorder\n\t"
        "lui\t$2,%%hi(D_8009B4B0)\n\t"
        "lw\t$2,%%lo(D_8009B4B0)($2)\n\t"
        "addiu\t$sp,$sp,-0x10\n\t"
        "sh\t$zero,0xA($2)\n\t"
        "addiu\t$2,$zero,0xA\n\t"
        "sw\t$2,0($sp)\n\t"
        "lw\t$2,0($sp)\n\t"
        "nop\n\t"
        "addiu\t$2,$2,-1\n\t"
        "sw\t$2,0($sp)\n\t"
        "lw\t$3,0($sp)\n\t"
        "addiu\t$2,$zero,-1\n\t"
        "beq\t$3,$2,2f\n\t"
        "addu\t$2,$zero,$zero\n\t"
        "addiu\t$3,$zero,-1\n"
        "1:\n\t"
        "lw\t$2,0($sp)\n\t"
        "nop\n\t"
        "addiu\t$2,$2,-1\n\t"
        "sw\t$2,0($sp)\n\t"
        "lw\t$2,0($sp)\n\t"
        "nop\n\t"
        "bne\t$2,$3,1b\n\t"
        "addu\t$2,$zero,$zero\n"
        "2:\n\t"
        "addiu\t$sp,$sp,0x10\n\t"
        ".set\treorder"
        :
        :
        : "$2", "$3", "memory");
}

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
