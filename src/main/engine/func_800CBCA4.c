typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define PTR(base, off) (*(u8 **)((u8 *)(base) + (off)))

extern u32 *FieldEng_GetSlot(int slot);

extern u8 D_800E0D6C[];
extern volatile s16 D_800E2290;
extern volatile s16 D_800E2292;
extern volatile s16 D_800E2294;
extern volatile u8 D_800E2284;
extern volatile u8 D_800E2285;
extern volatile s16 D_800E2286;
extern volatile s16 D_800E2268;
extern volatile s16 D_800E226A;
extern volatile s16 D_800E226C;
extern volatile s16 D_800E2820;
extern volatile s16 D_800E2822;
extern volatile s16 D_800E2824;
extern volatile u32 D_800E2828;
extern volatile u32 D_800E282C;
extern volatile u32 D_800E2830;
extern volatile u8 D_800E2838;
extern volatile u8 D_800E2839;
extern volatile u8 D_800E283A;
extern volatile u8 D_800E283C;
extern volatile u8 D_800E283D;
extern volatile s16 D_800E283E;

extern volatile u8 *D_800F3470;
extern volatile u8 D_800F3454;
extern volatile u8 D_800F3455;
extern volatile s16 D_800F3456;
extern volatile s16 D_800F3438;
extern volatile s16 D_800F343A;
extern volatile s16 D_800F343C;
extern volatile s16 D_800F32E8;
extern volatile s16 D_800F32EA;
extern volatile s16 D_800F32EC;
extern volatile u8 D_800F3300;
extern volatile u8 D_800F3301;
extern volatile u8 D_800F3302;
extern volatile u8 D_800F3304;
extern volatile u8 D_800F3305;
extern volatile s16 D_800F3306;
extern volatile s16 D_800F3388;
extern volatile s16 D_800F338A;
extern volatile s16 D_800F338C;
extern volatile u8 D_800F33A4;
extern volatile u8 D_800F33A5;
extern volatile s16 D_800F33A6;
extern volatile u8 D_800F335C;
extern volatile u8 D_800F335D;
extern volatile s16 D_800F335E;
extern volatile u8 D_800F33A0;
extern volatile u8 D_800F33A1;
extern volatile u8 D_800F33A2;
extern volatile s16 D_800F3340;
extern volatile s16 D_800F3342;
extern volatile s16 D_800F3344;
extern volatile u8 D_800F3358;
extern volatile u8 D_800F3359;
extern volatile u8 D_800F335A;

int func_800CBCA4(u8 *arg0) {
    register u8 *obj asm("$3");
    register int tmp asm("$2");
    register int y_scale asm("$5");
    register int color asm("$3");
    register int z asm("$6");
    register int orange asm("$4");
    register int y_off asm("$4");
    u32 *slot;

    slot = FieldEng_GetSlot((int)arg0);
    *slot = (u32)D_800E0D6C;
    obj = PTR(arg0, 8);

    tmp = S16(obj, 0x2A);
    y_off = -0x64;
    D_800E2290 = tmp;
    tmp = S16(obj, 0x2E);
    y_scale = 0x400;
    D_800F3470 = obj;
    D_800E2292 = tmp;

    tmp = 0;
    asm volatile("" : "=r"(tmp) : "0"(tmp) : "memory");
    z = S16(obj, 0x32);
    color = 0x68;
    D_800F3454 = color;
    D_800E2284 = color;
    D_800F3304 = color;

    color = 0xA0;
    D_800F3300 = color;
    D_800F3301 = color;
    D_800F3302 = color;

    color = 0x46;
    D_800F3456 = y_off;
    D_800F3306 = y_off;

    orange = 0x80;
    D_800F3438 = 0;
    D_800F343A = 0;
    D_800F343C = 0;
    D_800F3455 = 0;

    D_800E2268 = y_scale;
    D_800E226A = 0;
    D_800E226C = 0;
    D_800E2285 = 0;
    D_800E2286 = 0;

    D_800F32E8 = y_scale;
    D_800F32EA = 0;
    D_800F32EC = 0;
    D_800F3305 = 0;

    D_800F3388 = y_scale;
    D_800F338A = 0;
    D_800F338C = 0;

    D_800E2294 = z;
    D_800F33A4 = color;

    color = 0x30;
    D_800F33A5 = color;

    color = -0x50;
    D_800F33A6 = color;

    color = 0x1A8;
    D_800E2828 = color;
    D_800E282C = color;
    D_800E2830 = color;

    color = 0xBC;
    D_800E283C = color;

    color = 8;
    D_800E283D = color;

    color = 0x42;
    D_800F335C = color;

    color = 0x20;
    D_800F335D = color;

    color = 0x32;
    D_800F33A0 = orange;
    D_800F33A1 = orange;
    D_800F33A2 = orange;

    D_800E2820 = 0;
    D_800E2822 = 0;
    D_800E2824 = 0;
    D_800E283E = 0;
    D_800E2838 = orange;
    D_800E2839 = orange;
    D_800E283A = orange;

    D_800F3340 = y_scale;
    D_800F3342 = 0;
    D_800F3344 = 0;
    D_800F335E = color;
    D_800F3358 = orange;
    D_800F3359 = orange;
    D_800F335A = orange;
    return tmp;
}
