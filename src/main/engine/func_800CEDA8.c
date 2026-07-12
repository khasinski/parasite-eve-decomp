typedef signed short s16;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

int LoadImage(RECT *rect, void *pixels);

extern void *D_800B0E18;
extern void *D_800B0E1C;
extern s16 D_800F34E4;

void func_800CEDA8(int index) {
    RECT rect;
    void *pixels;

    if (D_800F34E4 != index) {
        rect.x = 0x380;
        rect.y = 0x100;
        rect.w = 0x40;
        rect.h = 0x100;

        if (index == 0) {
            pixels = D_800B0E18;
        } else {
            pixels = D_800B0E1C;
        }

        LoadImage(&rect, pixels);
        D_800F34E4 = index;
    }
}
