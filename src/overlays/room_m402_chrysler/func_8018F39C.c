#include "common.h"
#include "pe1/room_effect_state.h"

extern u8 D_80194390;
extern u8 D_80194391;
extern u8 D_80194392;
extern u8 D_80194394;
extern u8 D_80194395;
extern u8 D_80194396;
extern s16 D_80194398;
extern s16 D_8019439A;
extern u8 D_801943A0;
extern u8 D_801943A1;
extern u8 D_801943A2;
extern u8 D_801943A4;
extern u8 D_801943A5;
extern u8 D_801943A6;
extern s16 D_801943A8;
extern s16 D_801943AA;

extern void func_800C2B40(void *arg0);
extern s32 *func_800C2B28(int arg0);
extern void *func_8006DC18(int type);

void func_8018F39C(char *obj, void *arg1, char *state) {
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

    D_801943A8 = -0x64;
    D_80194394 = 4;
    D_80194395 = 1;
    D_801943A4 = 0;
    D_801943A5 = 0;
    D_801943AA = 0x80;
    D_801943A0 = 0x80;
    D_801943A1 = 0x80;
    D_801943A2 = 0x80;
    D_801943A6 = 0;
    D_80194398 = 0x32;
    D_8019439A = 0x80;
    D_80194390 = 0x80;
    D_80194391 = 0x80;
    D_80194392 = 0x80;
    D_80194396 = 0;
}
