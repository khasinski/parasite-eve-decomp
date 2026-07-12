typedef unsigned char u8;
typedef signed short s16;

void **FieldEng_GetSlot(void);

extern int D_800E0D6C;
extern s16 D_800E2290;
extern s16 D_800E2292;
extern s16 D_800E2294;
extern void *D_800F3470;

extern u8 D_800F3454;
extern u8 D_800F3455;
extern s16 D_800F3456;
extern s16 D_800F3438;
extern s16 D_800F343A;
extern s16 D_800F343C;

extern u8 D_800E2284;
extern u8 D_800E2285;
extern s16 D_800E2286;
extern s16 D_800E2268;
extern s16 D_800E226A;
extern s16 D_800E226C;

extern u8 D_800F3300;
extern u8 D_800F3301;
extern u8 D_800F3302;
extern u8 D_800F3304;
extern u8 D_800F3305;
extern s16 D_800F3306;
extern s16 D_800F32E8;
extern s16 D_800F32EA;
extern s16 D_800F32EC;
extern s16 D_800F3388;
extern s16 D_800F338A;
extern s16 D_800F338C;

extern u8 D_800F33A0;
extern u8 D_800F33A1;
extern u8 D_800F33A2;
extern u8 D_800F33A4;
extern u8 D_800F33A5;
extern s16 D_800F33A6;

extern int D_800E2828;
extern int D_800E282C;
extern int D_800E2830;
extern u8 D_800E2838;
extern u8 D_800E2839;
extern u8 D_800E283A;
extern u8 D_800E283C;
extern u8 D_800E283D;
extern s16 D_800E2820;
extern s16 D_800E2822;
extern s16 D_800E2824;
extern s16 D_800E283E;

extern u8 D_800F3358;
extern u8 D_800F3359;
extern u8 D_800F335A;
extern u8 D_800F335C;
extern u8 D_800F335D;
extern s16 D_800F335E;
extern s16 D_800F3340;
extern s16 D_800F3342;
extern s16 D_800F3344;

void func_800CBCA4(void *obj) {
    char *data;
    int neg100 = -0x64;
    int wide = 0x400;
    int full;

    *FieldEng_GetSlot() = &D_800E0D6C;

    data = *(char **)((char *)obj + 8);
    D_800E2290 = *(s16 *)(data + 0x2A);
    D_800F3470 = data;
    D_800E2292 = *(s16 *)(data + 0x2E);

    D_800F3454 = 0x68;
    D_800E2284 = 0x68;
    D_800F3304 = 0x68;
    D_800F3300 = 0xA0;
    D_800F3301 = 0xA0;
    D_800F3302 = 0xA0;
    D_800F3456 = neg100;
    D_800F3306 = neg100;
    D_800F3438 = 0;
    D_800F343A = 0;
    D_800F343C = 0;
    D_800F3455 = 0;
    D_800E2268 = wide;
    D_800E226A = 0;
    D_800E226C = 0;
    D_800E2285 = 0;
    D_800E2286 = 0;
    D_800F32E8 = wide;
    D_800F32EA = 0;
    D_800F32EC = 0;
    D_800F3305 = 0;
    D_800F3388 = wide;
    D_800F338A = 0;
    D_800F338C = 0;
    D_800E2294 = *(s16 *)(data + 0x32);
    D_800F33A4 = 0x46;
    D_800F33A5 = 0x30;
    D_800F33A6 = -0x50;
    D_800E2828 = 0x1A8;
    D_800E282C = 0x1A8;
    D_800E2830 = 0x1A8;
    D_800E283C = 0xBC;
    D_800E283D = 8;
    D_800F335C = 0x42;
    D_800F335D = 0x20;

    full = 0x80;
    D_800F33A0 = full;
    D_800F33A1 = full;
    D_800F33A2 = full;
    D_800E2820 = 0;
    D_800E2822 = 0;
    D_800E2824 = 0;
    D_800E283E = 0;
    D_800E2838 = full;
    D_800E2839 = full;
    D_800E283A = full;
    D_800F3340 = wide;
    D_800F3342 = 0;
    D_800F3344 = 0;
    D_800F335E = 0x32;
    D_800F3358 = full;
    D_800F3359 = full;
    D_800F335A = full;
}
