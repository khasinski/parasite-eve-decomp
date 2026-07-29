#include "pe1/room_effect_state.h"

typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern u8 D_80194858;
extern u8 D_80194859;
extern u8 D_8019485A;
extern u8 D_8019485C;
extern u8 D_8019485D;
extern u8 D_8019485E;
extern s16 D_80194860;
extern s16 D_80194862;
extern u8 D_80194868;
extern u8 D_80194869;
extern u8 D_8019486A;
extern u8 D_8019486C;
extern u8 D_8019486D;
extern u8 D_8019486E;
extern s16 D_80194870;
extern s16 D_80194872;

extern void func_800C2B40(void *arg0);
extern s32 *func_800C2B28(int arg0);
extern void *func_8006DC18(int type);

void func_8018F3CC(char *obj, void *arg1, char *state) {
    char *owner;
    RoomEffectWords8 *src;

    func_800C2B40(state);
    *(void **)(state + 0x2C) = func_8006DC18(0xA);

    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    src = *(RoomEffectWords8 **)(owner + 0x238);
    *(RoomEffectWords8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x26) = 0;
    *(s16 *)(state + 0x28) = 0;
    *(s16 *)(state + 0x24) = *func_800C2B28(6);

    D_80194870 = -0x64;
    D_8019485C = 4;
    D_8019485D = 1;
    D_8019486C = 0;
    D_8019486D = 0;
    D_80194872 = 0x80;
    D_80194868 = 0x80;
    D_80194869 = 0x80;
    D_8019486A = 0x80;
    D_8019486E = 0;
    D_80194860 = 0x32;
    D_80194862 = 0x80;
    D_80194858 = 0x80;
    D_80194859 = 0x80;
    D_8019485A = 0x80;
    D_8019485E = 0;
}
