#ifndef ROOMLIB_INIT_PAIRED_EMITTER_NAME
#define ROOMLIB_INIT_PAIRED_EMITTER_NAME func_8018FBB0
#endif

extern int *func_800C2B10(int index);
extern void func_800C4E50(void *params);

void ROOMLIB_INIT_PAIRED_EMITTER_NAME(void *unused0, void *unused1,
                                     RoomFxPairedEmitterState *state) {
    int *value;
    unsigned char *source;

    state->phase = 0;
    state->timer = 0;
    state->intensity = 0x80;
    state->primary.mode = 0x10;
    state->primary.offset = 0;
    state->primary.intensity = 0x80;

    value = func_800C2B10(1);
    state->primary.color1[0] = *value;
    value = func_800C2B10(2);
    state->primary.color1[1] = *value;
    value = func_800C2B10(3);
    state->primary.color1[2] = *value;
    value = func_800C2B10(4);
    state->primary.color0[0] = *value;
    value = func_800C2B10(5);
    state->primary.color0[1] = *value;
    value = func_800C2B10(6);
    state->primary.color0[2] = *value;

    source = state->sourceData;
    state->primary.extent0 = 0x4B0;
    state->primary.extent1 = 0x258;
    state->primary.source = source;
    func_800C4E50(&state->primary);

    state->secondary.mode = 0x10;
    state->secondary.offset = 0;
    state->secondary.intensity = 0x80;

    value = func_800C2B10(1);
    state->secondary.color1[0] = *value >> 1;
    value = func_800C2B10(2);
    state->secondary.color1[1] = *value >> 1;
    value = func_800C2B10(3);
    state->secondary.color1[2] = *value >> 1;
    value = func_800C2B10(4);
    state->secondary.color0[0] = *value >> 1;
    value = func_800C2B10(5);
    state->secondary.color0[1] = *value >> 1;
    value = func_800C2B10(6);
    state->secondary.color0[2] = *value >> 1;

    state->secondary.extent0 = 0x4B0;
    state->secondary.extent1 = 0x12C;
    state->secondary.source = source;
    func_800C4E50(&state->secondary);
}

#undef ROOMLIB_INIT_PAIRED_EMITTER_NAME
