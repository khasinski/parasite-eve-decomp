extern short *D_8009B4B0;

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
