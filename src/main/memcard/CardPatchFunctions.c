/* CC1_FLAGS: -g3 -G8 */
typedef unsigned int u32;

asm(".globl func_8007E370");
asm("func_8007E370 = CardPatchFunctions");

void CardPatchFunctions(void) {
    volatile u32 *v1;

    if ((v1[0x1074 / 4] & 0x80) != 0) {
        while ((v1[0x1044 / 4] & 0x80) != 0) {
        }
        __asm__ volatile(
            "lui $2,0x1\n\t"
            "lw $2,-0x2004($2)\n\t"
            "nop\n\t"
            "jr $2\n\t"
            "nop");
    }
}

void __maspsx_include_asm_hack_CardPatchFunctionsTail(void) {
    asm(
        ".text # maspsx-keep\n"
        ".include \"src/main/memcard/CardPatchFunctions.inc.s\" # maspsx-keep\n"
    );
}
