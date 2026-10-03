#include "pe1/psyq_cd.h"
void CD_flush(void);

extern s32 D_8009B59C[];
#define D_8009B59C (D_8009B59C[0])

s32 CdRom_RetryCmd(void) {
    void *base;
    s32 value;
    register s32 idx asm("$3");
    s32 arg0;
    s32 arg2;
    s32 arg3;

    CD_flush();
    base = &D_8009B59C;
    asm volatile("" : "=r"(base) : "0"(base));
    value = *(s32 *)base;
    idx = *(u8 *)((char *)base - 0x44);
    value += 1;
    idx = idx << 2;
    *(s32 *)base = value;
    asm volatile("" : : "r"(value) : "memory");
    {
        /* g_CdRomCmdLongTimeoutTable is at 0x8009B5A4 in the USA image. */
        u32 table_page = 0x800A0000u;
        asm volatile("" : "=r"(table_page) : "0"(table_page));
        value = *(s32 *)(table_page + idx - 0x4A5Cu);
    }
    idx = 0x1E;
    if (value != 0) {
        idx = 0x3C0;
    }
    arg0 = *(u8 *)((char *)base - 0x44);
    arg2 = 0;
    *(s32 *)((char *)base - 4) = idx;
    value = *(s32 *)((char *)base - 0x3C);
    arg3 = 1;
    CD_cw(arg0, (void *)value, (u8 *)arg2, arg3);
    return 0;
}
