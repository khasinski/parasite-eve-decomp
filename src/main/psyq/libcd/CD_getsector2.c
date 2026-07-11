typedef unsigned char u8;
typedef unsigned int u32;

extern volatile u8 *D_8009B27C;
extern volatile u8 *D_8009B288;
extern volatile u32 *D_8009B28C;
extern volatile u32 *D_8009B2B0;
extern volatile u32 *D_8009B2B4;
extern volatile u32 *D_8009B2B8;
extern volatile u32 *D_8009B2BC;
extern volatile u32 *D_8009B2C0;

int CD_getsector2(u32 arg0, u32 arg1) {
    register volatile u8 *status asm("$3");
    register u32 dmaCommand asm("$2");

    *D_8009B27C = 0;
    *D_8009B288 = 0x80;
    *D_8009B2B0 = 0x20943;
    *D_8009B28C = 0x1323;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = arg0;
    *D_8009B2BC = arg1 | 0x10000;

    status = D_8009B27C;
    asm volatile(
        ".set noreorder\n\t"
        "nop\n\t"
        "1:\n\t"
        "lbu %0,0(%1)\n\t"
        "nop\n\t"
        "andi %0,%0,0x40\n\t"
        "beqz %0,1b\n\t"
        "lui %0,0x1100\n\t"
        ".set noreorder"
        : "=r"(dmaCommand)
        : "r"(status)
        : "memory");

    *D_8009B2C0 = dmaCommand;
    asm volatile(
        ".set noreorder\n\t"
        "lui $4,%%hi(D_8009B2C0)\n\t"
        "lw $4,%%lo(D_8009B2C0)($4)\n\t"
        "nop\n\t"
        "lw $2,0($4)\n\t"
        "lui $3,0x100\n\t"
        "and $2,$2,$3\n\t"
        "beqz $2,2f\n\t"
        "addu $3,$4,$0\n\t"
        "lui $4,0x100\n\t"
        "1:\n\t"
        "lw $2,0($3)\n\t"
        "nop\n\t"
        "and $2,$2,$4\n\t"
        "bnez $2,1b\n\t"
        "nop\n\t"
        "2:\n\t"
        ".set noreorder"
        :
        :
        : "$2", "$3", "$4", "memory");

    {
        register volatile u32 *request asm("$3") = D_8009B28C;
        register u32 requestValue asm("$2") = 0x1325;

        *request = requestValue;
        asm volatile("" : : "r"(request), "r"(requestValue) : "memory");
    }
    return 0;
}
