#include "../room_lib/room_lib.h"

extern void D_80192688(void);
extern char D_8019B2FC[];
extern int D_8019B310;
extern int D_8019B314;
extern int D_8019B318;
extern char D_8019B31C[];

void func_80079754(void *src, void *dst);
void func_800785D4(void *src, void *dst, void *out);

int func_8019259C(void *obj) {
    register int last asm("$3");
    register int ret asm("$2");

    RW32(obj, 0xC) = (int)&D_80192688;
    RW8(obj, 0x1A) = 0;
    RW8(obj, 0x44) = 0;
    RW8(obj, 3) = 0xFF;
    func_80079754(D_8019B31C, (void *)0x1F800028);
    func_800785D4(D_8019B2FC, (void *)0x1F800028, (char *)obj + 0x1C);
    RW32(obj, 0x30) = D_8019B310;
    RW32(obj, 0x34) = D_8019B314;
    last = D_8019B318;
    ret = 0;
    RW32(obj, 0x38) = last;
    return ret;
}
