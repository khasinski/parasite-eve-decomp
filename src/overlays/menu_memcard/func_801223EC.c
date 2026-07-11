typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

extern s8 D_800B0DBB;
extern u8 D_801223F8;

void func_801223EC(s32 arg0) {
    arg0 <<= 0x18;
    if (arg0 != 0) {
        if (D_800B0DBB == 0) {
            D_801223F8 = 1;
            return;
        }
        asm volatile(
            ".set\tnoreorder\n\t"
            "j\tfunc_801223EC\n\t"
            "nop\n\t"
            ".set\treorder");
    }
    if (D_800B0DBB != 0) {
        D_801223F8 = 1;
    }
}
