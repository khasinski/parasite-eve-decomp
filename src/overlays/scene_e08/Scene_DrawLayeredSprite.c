#include "scene_e08.h"

typedef struct {
    s16 fraction;
    s16 integer;
} SceneFixedCoordinate;

typedef struct {
    s16 x, y, z, pad;
} SceneLayerPosition;

typedef struct {
    SceneFixedCoordinate center[3];
    u8 pad_0C[0x14];
    RoomFxSeed8 initialSeed;
    u8 pad_28[8];
    SceneLayerPosition layers[5];
    RoomFxSeed8 layerSeeds[5];
} SceneLayeredSpriteState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, initialSeed) == 0x20,
                  scene_layered_initial_seed_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, layers) == 0x30,
                  scene_layered_positions_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, layerSeeds) == 0x58,
                  scene_layered_seed_offset);

typedef struct {
    Ovl178Position position;
    u8 drawMode;
} SceneLayeredOwner;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredOwner, drawMode) == 0x24,
                  scene_layered_draw_mode_offset);

extern RoomFxVec4 D_8018F050;
extern u8 D_80199560, D_80199561, D_80199562;
extern s16 D_8019956A, D_800942EC;

void func_800C2EAC(u8);
void func_800C2FF0(int, int);
void func_800C3098(int);
void func_800C3238(int);
void func_800794C4(void *, RoomSpriteMatrix *);
void func_80078CC4(RoomSpriteMatrix *, RoomFxVec4 *);
void func_800C42A4(void *, RoomSpriteMatrix *, int);

void func_80194498(void *unused, u16 *phase, SceneLayeredSpriteState *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 firstScale, layerScale;
    char *owner;
    s16 *alpha;
    unsigned i;
    s16 *alphaBase;
    u8 *red;

    owner = (char *)func_800C2B50();
    func_800C2EAC(((SceneLayeredOwner *)owner)->drawMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    func_800794C4(&state->initialSeed, &matrix);
    firstScale = D_8018F050;
    func_80078CC4(&matrix, &firstScale);
    if (phase[1] & 1) {
        D_80199560 = 0xFF;
        D_80199561 = 0xA0;
    } else {
        D_80199560 = 0xA0;
        D_80199561 = 0xFF;
    }
    D_80199562 = 0x60;
    PE1_COMPILER_MEMORY_BARRIER();

    alphaBase = &D_8019956A;
    alpha = alphaBase;
    matrix.t[0] = state->center[0].integer;
    red = (u8 *)((char *)alpha - 10);
    matrix.t[1] = state->center[1].integer;
    matrix.t[2] = state->center[2].integer;
    *alpha = 0x80;
    func_800C42A4((char *)alphaBase - 10, &matrix, 0);

    matrix.t[0] = state->center[0].integer;
    matrix.t[1] = D_800942EC;
    matrix.t[2] = state->center[2].integer;
    *alpha = 0x20;
    func_800C42A4((char *)alpha - 10, &matrix, 0);

    *red = 0x40;
    D_80199561 = 0x20;
    D_80199562 = 0;
    for (i = 0; i < 5; ++i) {
        func_800794C4(&state->layerSeeds[i], &matrix);
        layerScale = D_8018F050;
        func_80078CC4(&matrix, &layerScale);
        owner = (char *)alpha;
        matrix.t[0] = state->layers[i].x;
        matrix.t[1] = state->layers[i].y;
        matrix.t[2] = state->layers[i].z;
        *alpha = (5 - i) * 31;
        func_800C42A4(owner - 10, &matrix, 0);
    }
}
