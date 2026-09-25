#include "scene_e08.h"

typedef struct SceneShiftedPosition {
    char reserved[0x14];
    s32 x, y, z;
} SceneShiftedPosition;

typedef struct SceneFadeOwner {
    u8 reserved[4];
    SceneShiftedPosition position;
    u8 drawMode;
} SceneFadeOwner;

typedef struct SceneDrawColor {
    u8 r, g, b, pad3, t4, t5, t6, pad7;
    s16 h8, hA;
} SceneDrawColor;

typedef union SceneFadeSeed {
    RoomFxSeed8 raw;
    struct {
        u8 initial[4];
        s16 angle;
        u8 tail[2];
    } fields;
} SceneFadeSeed;

extern RoomFxSeed8 D_8018EFF4;
extern SceneDrawColor D_80199670;
extern u8 D_801986F8[], D_80198708[];
void func_800C2EAC(u8);
void func_800C2FF0(int, int);
void func_800C3098(int);
void func_800C3238(int);
void func_800C3134(void *, int, void *);
void func_80071A44(RoomFxVec4 *, int, int);
void func_800794C4(RoomFxSeed8 *, RoomSpriteMatrix *);
void func_80078CC4(RoomSpriteMatrix *, RoomFxVec4 *);
void func_800C42A4(void *, RoomSpriteMatrix *, int);
void func_800C4FC4(void *, RoomSpriteMatrix *, int);

void func_8018F640(void *unused, s16 *args, Ovl178FadeState *state) {
    RoomSpriteMatrix matrix;
    SceneFadeSeed seed;
    RoomFxVec4 scale, sourceScale;
    SceneFadeOwner *owner;
    SceneShiftedPosition *ownerCursor;
    unsigned i;

    owner = (SceneFadeOwner *)func_800C2B50();
    seed.raw = D_8018EFF4;

    ownerCursor = &owner->position;
    func_800C2EAC(owner->drawMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C3134(D_801986F8, args[1], &D_80199670);
    for (i = 0; i < 4; ++i) {
        seed.fields.angle = state->field_25C[i];
        func_800794C4(&seed.raw, &matrix);
        func_80071A44(&sourceScale, 0, 0x10);
        sourceScale.x = state->field_254[i];
        sourceScale.y = state->field_254[i];
        sourceScale.z = state->field_254[i];
        scale = sourceScale;
        func_80078CC4(&matrix, &scale);
        matrix.t[0] = ownerCursor->x;
        matrix.t[1] = ownerCursor->y;
        matrix.t[2] = ownerCursor->z;
        D_80199670.hA = state->field_264;
        func_800C42A4(&D_80199670, &matrix, 1);
    }
    for (i = 0; i < 2; ++i) {
        matrix.m[2][2] = 0x1000;
        matrix.m[1][1] = 0x1000;
        matrix.m[0][0] = 0x1000;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][0] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][2] = 0;
        matrix.m[0][1] = 0;
        func_80071A44(&sourceScale, 0, 0x10);
        sourceScale.x = state->field_230[i];
        sourceScale.y = state->field_230[i];
        sourceScale.z = state->field_230[i];
        scale = sourceScale;
        func_80078CC4(&matrix, &scale);
        matrix.t[0] = ownerCursor->x;
        matrix.t[1] = ownerCursor->y;
        matrix.t[2] = ownerCursor->z;
        func_800C3134(D_80198708, args[1], &state->emitters[i].color1[0]);
        func_800C4FC4(&state->emitters[i], &matrix, 1);
    }
}
