typedef short s16;
typedef unsigned short u16;

typedef struct RECT {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

extern u16 D_800F336C;
extern u16 D_800E1204[];
extern u16 D_800E21A8[];

int StoreImage(RECT *rect, void *pixels);
int LoadImage(RECT *rect, void *pixels);

void func_800CF4B4(int arg0, int arg1, u16 *pixels) {
    RECT rect;
    int i;
    u16 *dst;
    int magic;
    int source_offset;

    rect.x = (arg0 & 0xF) << 4;
    rect.y = D_800E1204[D_800F336C] + (arg0 / 16);
    rect.w = 0x10;
    rect.h = 1;

    if (arg1 == -1) {
        StoreImage(&rect, pixels);
        return;
    }

    i = 1;
    magic = 0x88888889;
    dst = D_800E21A8;
    dst[0] = pixels[0];
    do {
        source_offset = (u16)i * 2;
        i++;
        dst[(arg1 % 15) + 1] = *(u16 *)(source_offset + (int)pixels);
        arg1++;
    } while ((unsigned int)(u16)i < 0x10U);

    LoadImage(&rect, dst);
}
