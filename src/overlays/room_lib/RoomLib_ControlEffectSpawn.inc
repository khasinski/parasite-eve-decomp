#include "common.h"
#include "room_lib.h"

extern s32 D_800E27EC;
extern u16 D_800E11EA;
extern u16 D_800E2850[];
extern u16 D_800F336C;
extern s16 D_800F336E;
extern u16 D_800F3370;
extern s16 D_800F3372;
extern s16 D_800F3374;

extern void ROOMLIB_EFFECT_SPAWN_CALLBACK(void);

int ROOMLIB_CONTROL_EFFECT_SPAWN_NAME(int mode, u16 *value) {
    RoomQRec *record;
    RoomDlgState *ui;
    u16 texture;

    if (mode == 1) {
        goto mode1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto mode0;
        }
        goto ret0;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto ret0;

mode0:
    ui = D_800E2368;
    *value = 0;
    if (ui->bD != 0) {
        RoomNodeB **nodes = D_800F32D0->w8;
        if (nodes != 0 && *nodes != 0) {
            unsigned char *state = (*nodes)->state;
            if (*state == 1) {
                *state = 2;
            }
        }
    }
    return func_800CE560(D_800F33E0->w8, 0x18, 0x20,
                         ROOMLIB_EFFECT_SPAWN_CALLBACK);

mode1:
    if (D_800E27EC == mode) {
        record = func_800CE610(D_800F33E0->w8);
        if (record != 0) {
            record->h6 = *value;
            *(s16 *)((char *)record + 0x14) = 0;
            *(s16 *)((char *)record + 0x16) = 0;
        }
    }
    if (D_800E27EC < 8) {
        goto ret0;
    }
    return 2;

mode2:
    texture = D_800E2850[D_800E11EA];
    PE1_COMPILER_MEMORY_BARRIER();
    D_800F336C = 3;
    D_800F336E = 0;
    D_800F3372 = 0;
    D_800F3374 = 8;
    D_800F3370 = texture;
ret0:
    return 0;
}
