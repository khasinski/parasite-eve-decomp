typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);
int *func_800C2B10(int index);

extern u8 D_800F3450;
extern u8 D_800F3451;
extern u8 D_800F3452;
extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;

int func_800CBFC4(void *arg0, void *arg1, u8 *anim) {
    int x_base_a1;
    int z_base_a1;
    int x_rand;
    int z_rand;

    D_800F3450 = *func_800C2B10(0);
    D_800F3451 = *func_800C2B10(1);
    D_800F3452 = *func_800C2B10(2);

    x_rand = rand();
    x_base_a1 = D_800E2290 - 0x64;
    *(u16 *)(anim + 0x6) = x_base_a1 + (x_rand % 201);
    *(u16 *)(anim + 0x8) = D_800E2292;

    z_rand = rand();
    z_base_a1 = D_800E2294 - 0x64;
    *(u16 *)(anim + 0x4) = 0x20C;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z_base_a1 + (z_rand % 201);
}
