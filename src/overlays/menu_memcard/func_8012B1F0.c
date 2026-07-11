typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

extern s8 D_800B0DBB;
extern u8 D_801D0DC0;

void func_8012B1F0(s32 arg0) {
    arg0 <<= 0x18;
    if (arg0 != 0) {
        if (D_800B0DBB == 0) {
            D_801D0DC0 = 1;
            return;
        }
        asm volatile(
            ".set\tnoreorder\n\t"
            "j\tfunc_80192CE0\n\t"
            "nop\n\t"
            ".set\treorder");
    }
    if (D_800B0DBB != 0) {
        D_801D0DC0 = 1;
    }
}
