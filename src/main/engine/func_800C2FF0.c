typedef unsigned char u8;
typedef signed short s16;

extern u8 D_800F345C;
extern u8 D_800F345D;
extern s16 D_800F3310;
extern s16 D_800F3312;
extern s16 D_800F3314;
extern s16 D_800F3318;
extern s16 D_800F331A;
extern s16 D_800F331C;
extern s16 D_800F3320;
extern s16 D_800F3322;
extern s16 D_800F3324;
extern s16 D_800F3328;
extern s16 D_800F332A;
extern s16 D_800F332C;

void func_800C2FF0(int width, int height) {
    register int widthMinus asm("$6");
    register int heightMinus asm("$7");
    register int x asm("$4");
    register int y asm("$5");

    asm volatile(
        "addiu %0,%2,-1\n\t"
        "addiu %1,%3,-1"
        : "=r"(widthMinus), "=r"(heightMinus)
        : "r"(width), "r"(height));

    D_800F345C = width;
    x = (width & 0xFF) << 4;
    D_800F345D = height;
    y = (height & 0xFF) << 4;

    D_800F3310 = -x;
    D_800F3312 = -y;
    D_800F3314 = 0;
    D_800F3318 = x;
    D_800F331A = -y;
    D_800F331C = 0;
    D_800F3320 = -x;
    D_800F3322 = y;
    D_800F3324 = 0;
    D_800F3328 = x;
    D_800F332A = y;
    D_800F332C = 0;

    D_800F345C = widthMinus;
    D_800F345D = heightMinus;
}
