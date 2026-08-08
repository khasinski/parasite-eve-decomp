#include "common.h"
#include "pe1/room_effect_state.h"

extern u8 D_801955C0;
extern u8 D_801955C1;
extern u8 D_801955C2;
extern u8 D_801955C4;
extern u8 D_801955C5;
extern u8 D_801955C6;
extern s16 D_801955C8;
extern s16 D_801955CA;
extern u8 D_801955D0;
extern u8 D_801955D1;
extern u8 D_801955D2;
extern u8 D_801955D4;
extern u8 D_801955D5;
extern u8 D_801955D6;
extern s16 D_801955D8;
extern s16 D_801955DA;

extern void func_800C2B40(void *arg0);
extern s32 *func_800C2B28(int arg0);
extern void *func_8006DC18(int type);

void func_8018F3D4(char *obj, void *arg1, char *state) {
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

    D_801955D8 = -0x64;
    D_801955C4 = 4;
    D_801955C5 = 1;
    D_801955D4 = 0;
    D_801955D5 = 0;
    D_801955DA = 0x80;
    D_801955D0 = 0x80;
    D_801955D1 = 0x80;
    D_801955D2 = 0x80;
    D_801955D6 = 0;
    D_801955C8 = 0x32;
    D_801955CA = 0x80;
    D_801955C0 = 0x80;
    D_801955C1 = 0x80;
    D_801955C2 = 0x80;
    D_801955C6 = 0;
}
