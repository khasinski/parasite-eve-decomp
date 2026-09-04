#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomPairedSpriteView {
    unsigned char pad0[0xA0];
    unsigned char transform[0x14];
    int baseX;
    int baseY;
    int baseZ;
} RoomPairedSpriteView;

typedef struct RoomPairedSpriteRoot {
    unsigned char pad0[0x238];
    RoomPairedSpriteView *view;
} RoomPairedSpriteRoot;

typedef struct RoomPairedSpriteState {
    short x;
    short y;
    short z;
    unsigned char pad6[2];
    short velocityX;
    short velocityY;
    short velocityZ;
    unsigned char padE[2];
    short scale;
    unsigned short depth;
    short phase;
    unsigned char active;
} RoomPairedSpriteState;

extern RoomFxSeed8 ROOMFX_INIT_PAIRED_SPRITE_POSITION_SEED;
extern RoomFxSeed8 ROOMFX_INIT_PAIRED_SPRITE_VELOCITY_SEED;

extern RoomPairedSpriteRoot **func_800C2B50(void);
extern int *func_800C2B10(int index);
extern void func_80078C34(
    void *transform, RoomFxSeed8 *seed, unsigned short *out);
extern int func_80071A54(void);
extern void func_800C6800(void *entity, int effectId, void *state);

void ROOMFX_INIT_PAIRED_SPRITE_NAME(
    void *entity, void *unused, RoomPairedSpriteState *state) {
    RoomFxSeed8 positionSeed;
    unsigned short position[4];
    RoomFxSeed8 velocitySeed;
    unsigned short velocity[4];
    RoomPairedSpriteRoot **root;
    RoomPairedSpriteView *view;

    root = func_800C2B50();
    positionSeed = ROOMFX_INIT_PAIRED_SPRITE_POSITION_SEED;
    velocitySeed = ROOMFX_INIT_PAIRED_SPRITE_VELOCITY_SEED;
    view = (*root)->view;
    func_80078C34(&view->transform, &positionSeed, position);

    state->x = position[0] + view->baseX;
    state->y = position[1] + view->baseY;
    state->z = position[2] + view->baseZ;
    state->scale = 0x100;
    state->depth = 0x80;
    state->phase = 0;
    state->active = 0;

    if (*func_800C2B10(1) == 0) {
        state->velocityX = func_80071A54() % 10 - 5;
        state->velocityY = 0;
        state->velocityZ = func_80071A54() % 10 - 5;
    } else {
        func_80078C34(&view->transform, &velocitySeed, velocity);
        state->velocityX = velocity[0] * 2;
        state->velocityY = 0;
        state->velocityZ = velocity[2] * 2;
    }

    func_800C6800(entity, 0x583, state);
}
