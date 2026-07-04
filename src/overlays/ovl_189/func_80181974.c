typedef signed short s16;
typedef int s32;

typedef struct {
    char pad0[0x28];
    char vector[0x12];
    s16 angle;
} Overlay189Actor;

typedef struct {
    char pad0[0x28];
    s32 source_x;
    char pad2C[0x4];
    s32 source_z;
} Overlay189Source;

typedef struct {
    char pad0[0x50];
    s32 target_x;
    char pad54[0x4];
    s32 target_z;
    char pad5C[0x8];
    Overlay189Source *source;
    char pad68[0x6];
    s16 step_count;
} Overlay189Track;

s32 func_800DFF80(void *from, void *to);
s32 func_800DFFB8(s32 current, s32 target, s32 steps);

void func_80181974(Overlay189Actor *actor, Overlay189Track *track) {
    Overlay189Source *source;
    s32 target_angle;

    if (track->step_count > 0) {
        source = track->source;
        if (source != 0) {
            track->target_x = source->source_x;
            track->target_z = source->source_z;
        }

        target_angle = func_800DFF80(&track->target_x, actor->vector);
        actor->angle = func_800DFFB8(actor->angle, (s16)target_angle, track->step_count);
    }
}
