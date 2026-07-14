#include "../room_lib/room_lib.h"

extern void func_80192638(void);
extern char D_8019A8B8[];
extern int D_8019A8CC;
extern int D_8019A8D0;
extern int D_8019A8D4;
extern char D_8019A8D8[];

void func_80079754(void *src, void *dst);
void func_800785D4(void *src, void *dst, void *out);

int func_8019250C(void *obj) {
    register int last asm("$3");
    register int ret asm("$2");

    RW32(obj, 0xC) = (int)&func_80192638;
    RW8(obj, 0x1A) = 0;
    RW8(obj, 0x4A) = 0;
    RW16(obj, 0x48) = 0;
    RW8(obj, 0x4B) = 0;
    RW8(obj, 3) = 0xFF;
    func_80079754(D_8019A8D8, (void *)0x1F800028);
    func_800785D4(D_8019A8B8, (void *)0x1F800028, (char *)obj + 0x1C);
    RW32(obj, 0x30) = D_8019A8CC;
    RW32(obj, 0x34) = D_8019A8D0;
    last = D_8019A8D4;
    ret = 0;
    RW32(obj, 0x38) = last;
    return ret;
}
