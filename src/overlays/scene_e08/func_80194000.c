typedef short s16;
typedef unsigned short u16;
typedef int s32;

char *func_800C2B50(void);
void func_800C2B10(s32 arg0);

void func_80194000(s32 arg0, s32 arg1, char *p) {
    char *view;
    s32 z;

    view = func_800C2B50();
    func_800C2B10(1);
    *(s16 *)(p + 0x8C) = 0x14;
    *(s16 *)(p + 0x88) = 0x80;
    *(s16 *)(p + 0x8A) = 0;
    *(s16 *)(p + 0x30) = 0;
    *(s16 *)(p + 0x32) = 0x400;
    *(s16 *)(p + 0x34) = 0;
    *(s32 *)(p + 0x0) = *(s32 *)(view + 0x18) - 0x15E;
    *(s32 *)(p + 0x4) = *(s32 *)(view + 0x1C) + 0x78;
    z = *(s32 *)(view + 0x20);
    *(s16 *)(p + 0x48) = 0;
    *(s16 *)(p + 0x4A) = 0x800;
    *(s16 *)(p + 0x4C) = 0;
    *(s32 *)(p + 0x8) = z;
}
