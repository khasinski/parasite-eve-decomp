#include "common.h"
#include "pe1/psyq_gpu.h"

int GetTPage(int tp, int abr, int x, int y);

extern short D_800E2850[];

void func_800CECAC(void) {
    int i;

    for (i = 0; i < 2; i++) {
        *(short *)((char *)D_800E2850 + i * 4) = GetTPage(i, 0, 0x380, 0x100);
    }

    for (i = 0; i < 2; i++) {
        *(short *)((char *)D_800E2850 + i * 4 + 2) = GetTPage(i, 0, 0x340, 0x100);
    }
}

int LoadImage(RECT *rect, void *pixels);

extern void *D_800B0E18;
extern void *D_800B0E1C;
extern s16 D_800F34E4;

void func_800CED3C(int index) {
    RECT rect;
    void *pixels;

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
