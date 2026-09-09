#include "common.h"
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
    {
        volatile u32 *control = D_8009B2C0;
        if (*control & 0x1000000) {
            volatile u32 *pending = control;
            u32 mask = 0x1000000;
            do {
            } while (*pending & mask);
        }
    }

    {
        volatile u32 *request = D_8009B28C;
        register u32 requestValue asm("$2") = 0x1325;

        *request = requestValue;

    }
    return 0;
}
