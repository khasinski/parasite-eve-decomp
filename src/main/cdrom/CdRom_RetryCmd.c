/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef unsigned char u8;
typedef int s32;

void CD_flush(void);
void CD_cw(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

extern s32 D_8009B59C[];
#define D_8009B59C (D_8009B59C[0])
extern s32 g_CdRomCmdLongTimeoutTable[];
#define g_CdRomCmdLongTimeoutTable (g_CdRomCmdLongTimeoutTable[0])

s32 CdRom_RetryCmd(void) {
    void *base;
    s32 value;
    register s32 idx asm("$3");
    s32 arg0;
    register s32 arg2 asm("$6");
    s32 arg3;

    CD_flush();
    base = &D_8009B59C;
    asm volatile("" : "=r"(base) : "0"(base));
    value = *(s32 *)base;
    idx = *(u8 *)((char *)base - 0x44);
    value += 1;
    idx = idx << 2;
    *(s32 *)base = value;
    asm volatile("lui %0,%%hi(g_CdRomCmdLongTimeoutTable)\n\taddu %0,%0,%1\n\tlw %0,%%lo(g_CdRomCmdLongTimeoutTable)(%0)" : "=r"(value) : "r"(idx));
    idx = 0x1E;
    if (value != 0) {
        idx = 0x3C0;
    }
    arg0 = *(u8 *)((char *)base - 0x44);
    arg2 = 0;
    *(s32 *)((char *)base - 4) = idx;
    value = *(s32 *)((char *)base - 0x3C);
    arg3 = 1;
    CD_cw(arg0, value, arg2, arg3);
    return 0;
}
