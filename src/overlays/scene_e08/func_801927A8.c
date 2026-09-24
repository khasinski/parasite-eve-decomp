#include "common.h"

extern u8 D_80198820[];
extern s16 D_800F3374;

void *func_800C2B50(void);
void func_800C2EAC(u8);
void func_800C3098(int);
void func_800C3238(int);
void func_800C3134(void *, int, void *);
int func_80077A64(int, int, int, int);
int func_80077AA4(int, int);
void func_800D3114(void *, int, int, int, int, int, int, int, int, int,
                   void *, void *, int);

void func_801927A8(void *unused0, void *unused1, char *effect) {
    unsigned int i;
    char *dst, *step, *frame;
    char *owner;
    u8 scratchA[8];
    u8 scratchB[8];
    register u8 *scratchPtr asm("$21");
    register u8 *table asm("$22");
    int retA, retB;

    owner = func_800C2B50();
    i = 0;
    table = D_80198820;
    scratchPtr = scratchB;
    func_800C2EAC(*(u8 *)(owner + 0x24));
    func_800C3098(0x10);
    func_800C3238(2);

    dst = effect;
    asm("" : : "r"(dst));
    frame = dst;
    step = frame;
    do {
        asm("" : "=r"(dst), "=r"(step), "=r"(frame)
               : "0"(dst), "1"(step), "2"(frame));
        if (*(s16 *)(frame + 0x1C0)) {
            func_800C3134(table, *(int *)(step + 0x180), scratchA);
            func_800C3134(table, *(int *)(step + 0x180), scratchPtr);
            scratchB[0] = 0;
            scratchB[1] = 0;
            scratchB[2] = 0;
            D_800F3374 = 0;
            retA = func_80077A64(1, 0, 0x340, 0x100) & 0xffff;
            retB = func_80077AA4(0, 0x1D9) & 0xffff;
            func_800D3114(dst, 2, *(s16 *)(frame + 0x240), 0x20, 0x90,
                           0x51, 0x10, retA, retB, *(s16 *)(frame + 0x200),
                           scratchA, scratchPtr, 1);
        }
        dst += 0x18;
        step += 4;
        ++i;
        frame += 2;
    } while (i < 16);
}
