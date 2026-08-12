#include "common.h"
#include "pe1/room_fx.h"

extern void *D_800B0E64;

int *func_800C2B10(int index);
void func_800C4E50(RoomFxEmitterParams *params);
void func_8006DF50(void *owner, int soundId, int arg2, int volume, int pan);

void ROOMLIB_INIT_PAIRED_EMITTER_NAME(void *arg0, void *arg1,
                                      RoomFxPairedEmitterState *state) {
    RoomFxEmitterParams *primary;
    RoomFxEmitterParams *secondary;
    void *volatile *soundOwnerSlot;

    state->phase = 0;
    state->timer = 0;
    state->intensity = 0x80;
    primary = &state->primary;
    primary->mode = 0x10;
    primary->offset = 0;
    primary->intensity = 0x80;
    primary->color1[0] = *func_800C2B10(1);
    primary->color1[1] = *func_800C2B10(2);
    primary->color1[2] = *func_800C2B10(3);
    primary->color0[0] = *func_800C2B10(4);
    primary->color0[1] = *func_800C2B10(5);
    primary->color0[2] = *func_800C2B10(6);
    primary->extent0 = 0x4B0;
    primary->extent1 = 0x258;
    primary->source = state->sourceData;
    func_800C4E50(primary);

    secondary = &state->secondary;
    secondary->mode = 0x10;
    secondary->offset = 0;
    secondary->intensity = 0x80;
    secondary->color1[0] = *func_800C2B10(1) >> 1;
    secondary->color1[1] = *func_800C2B10(2) >> 1;
    secondary->color1[2] = *func_800C2B10(3) >> 1;
    secondary->color0[0] = *func_800C2B10(4) >> 1;
    secondary->color0[1] = *func_800C2B10(5) >> 1;
    secondary->color0[2] = *func_800C2B10(6) >> 1;
    secondary->extent0 = 0x4B0;
    secondary->extent1 = 0x12C;
    secondary->source = state->sourceData;
    func_800C4E50(secondary);

    soundOwnerSlot = &D_800B0E64;
    if (*soundOwnerSlot != 0) {
        func_8006DF50(*soundOwnerSlot, 0x57E, 0, 0x80, 0x7F);
    }
}
