#include "common.h"

extern char D_8019C1F8[];
extern void *D_800B0E38, *D_800B0E3C;
extern void *D_8019C1FC, *D_8019C274;
extern u8 D_8019C218, D_8019C219, D_8019C21A, D_8019C21B;
extern u8 D_8019C290, D_8019C291, D_8019C292, D_8019C293;
extern void *D_8019C9C0;

void func_80074D28(int);
void func_80078FC4(int, int, int);
void func_80078FE4(int, int, int);
void func_80077E64(int, int, int);
void func_80079004(int, int);
void func_80079024(int);
void func_8019BF8C(void **);
void func_800752AC(void *, int);
void func_80074924(void *, int, int, int, int);
void func_800749D8(void *, int, int, int, int);
void func_80074F44(void *, int, int, int);
void func_80074DC0(int);

void func_8019BD78(void) {
    char *base = D_8019C1F8;
    s16 rect[4];

    func_80074D28(0);
    func_80078FC4(0x80, 0x80, 0x80);
    func_80078FE4(0, 0, 0);
    func_80077E64(0x1964, 0x2CEC, 0x300);
    func_80079004(0xA0, 0x78);
    func_80079024(0x300);

    func_8019BF8C((void **)base);
    func_8019BF8C((void **)(base + 0x78));
    D_8019C1FC = D_800B0E38;
    D_8019C274 = D_800B0E3C;
    func_800752AC(D_8019C1FC, 0x1000);
    func_800752AC(D_8019C274, 0x1000);

    func_80074924(base + 8, 0, 0, 0x140, 0xF0);
    func_800749D8(base + 0x64, 0, 0xF0, 0x140, 0xF0);
    func_80074924(base + 0x80, 0, 0xF0, 0x140, 0xF0);
    func_800749D8(base + 0xDC, 0, 0, 0x140, 0xF0);

    D_8019C218 = 0;
    D_8019C290 = 0;
    D_8019C219 = 0;
    D_8019C21A = 0;
    D_8019C21B = 0;
    D_8019C291 = 0;
    D_8019C292 = 0;
    D_8019C293 = 0;

    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x140;
    rect[3] = 0xF0;
    func_80074F44(rect, 0, 0, 0);

    rect[0] = 0;
    rect[1] = 0xF0;
    rect[2] = 0x140;
    rect[3] = 0xF0;
    func_80074F44(rect, 0, 0, 0);

    func_80074DC0(0);
    func_80074D28(1);
    D_8019C9C0 = base;
}
