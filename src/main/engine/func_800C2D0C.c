typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;

extern u8 *volatile D_800F34F4;
extern char *D_800E2248;

void func_800C2D0C(u16 slot, u8 id, int size) {
    {
        register char *state asm("$5");
        register u8 *entry asm("$7");
        register int offset asm("$4");

        asm volatile(
            "andi $4,$4,0xffff\n\t"
            "sll $3,$4,1\n\t"
            "addu $3,$3,$4\n\t"
            "sll $3,$3,1\n\t"
            "lui $2,%%hi(D_800F34F4)\n\t"
            "lw $2,%%lo(D_800F34F4)($2)\n\t"
            "li $4,1\n\t"
            "addu $2,$3,$2\n\t"
            "sb $4,1($2)\n\t"
            "lui $2,%%hi(D_800F34F4)\n\t"
            "lw $2,%%lo(D_800F34F4)($2)\n\t"
            "nop\n\t"
            "addu $2,$3,$2\n\t"
            "sb $5,0($2)\n\t"
            "lui $2,%%hi(D_800F34F4)\n\t"
            "lw $2,%%lo(D_800F34F4)($2)\n\t"
            "lui $5,%%hi(D_800E2248)\n\t"
            "lw $5,%%lo(D_800E2248)($5)\n\t"
            "addu $7,$3,$2\n\t"
            "sh $0,2($7)"
            : "=r"(entry), "=r"(state)
            :
            : "$2", "$3", "$4", "memory");

        asm volatile(
            "lh $2,4(%1)\n\t"
            "andi $3,$6,0xffff\n\t"
            "move %0,$2\n\t"
            "addu $2,$2,$3\n\t"
            "sltiu $2,$2,0x80c\n\t"
            "bnez $2,1f\n\t"
            "addiu $sp,$sp,-8\n\t"
            "move %0,$0\n"
            "1:"
            : "=r"(offset)
            : "r"(state)
            : "$2", "$sp");

        *(s16 *)(entry + 4) = offset;
        *(s16 *)(state + 4) = offset + size;
        state[6]++;
        asm volatile("addiu $sp,$sp,8" ::: "$sp");
    }
}
