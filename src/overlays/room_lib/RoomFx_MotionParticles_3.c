/*
 * The motion particle set, part 3 of 3: the particle motion update and the render pair
 * (init, draw, update).
 *
 * room_m075, room_m080, room_m082, scene_e09 and scene_e10 link the same
 * fifteen functions in the same order, after the room prelude and before
 * the room library, with the same 0x18 bytes of read-only data. The object
 * is split in three units only because its particle draw needs
 * -fno-strength-reduce, which the setup loop must not get; they are
 * merged once they compile together. Trails, glyphs and lookup codes live
 * in each room's own data (pe1/room_motion_init.h).
 */
#include "pe1/room_motion_init.h"
#include "pe1/room_ground_eruption.h"
#include "common.h"
#include "room_lib.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "RoomLib_RenderLayouts.h"
#include "RoomLib_DrawMotion.h"
#include "RoomSharedGlobalState.h"
#include "pe1/room_floor.h"


extern int func_80071A54(void);

typedef struct RoomRandomMotionView {
    char pad00[0x160];
    s16 x;
    s16 z;
} RoomRandomMotionView;

typedef struct RoomParticleMotionView {
    char pad00[0x40];
    u16 x0;
    u16 x1;
    u16 x2;
    u16 pad46;
    u16 x4;
    u16 x5;
    u16 x6;
    char pad4E[0x72];
    u16 v0;
    u16 v1;
    u16 v2;
    u16 padC6;
    u16 v4;
    u16 v5;
    u16 v6;
} RoomParticleMotionView;

void RoomFx_UpdateParticleMotion(int arg0, char *control, char *state) {
    int i;
    register RoomRandomMotionView *randomCursor asm("$16");
    register RoomParticleMotionView *motionCursor asm("$6");
    char *controlReg = control;
    char *stateReg = state;
    int loaded;
    int value;
    char scratch[0x18];

    PE1_COMPILER_LAUNDER_MEM(controlReg);

    i = 0;
    if (*(s16 *)(stateReg + 0x182) != 0) {
        randomCursor = (RoomRandomMotionView *)stateReg;
        do {
            randomCursor->x = func_80071A54() % 8;
            randomCursor->z = func_80071A54() % 8;
            loaded = *(s16 *)(stateReg + 0x182);
            PE1_COMPILER_MEMORY_BARRIER();
            i++;
            randomCursor =
                (RoomRandomMotionView *)((char *)randomCursor + 4);
        } while ((u32)i < loaded);
    }

    if (*(s16 *)(controlReg + 2) >= 31) {
        loaded = *(s16 *)(stateReg + 0x180);
        value = loaded;
        PE1_COMPILER_LAUNDER(value);
        if (loaded >= 5) {
            *(s16 *)(stateReg + 0x180) = value - 4;
        } else {
            *(s16 *)(stateReg + 0x180) = 0;
        }

        i = 0;
        if (*(s16 *)(stateReg + 0x182) != 0) {
            motionCursor = (RoomParticleMotionView *)stateReg;
            do {
                i++;
                motionCursor->x0 += motionCursor->v0;
                motionCursor->x1 += motionCursor->v1;
                motionCursor->x2 += motionCursor->v2;
                motionCursor->x4 += motionCursor->v4;
                motionCursor->x5 += motionCursor->v5;
                motionCursor->x6 += motionCursor->v6;
                motionCursor =
                    (RoomParticleMotionView *)((char *)motionCursor + 0x10);
            } while ((u32)i < *(s16 *)(stateReg + 0x182));
        }
    }

    if ((u32)(*(u16 *)(controlReg + 2) - 1) < 31) {
        *(u16 *)(stateReg + 0x180) += 4;
    }
    if (*(s16 *)(controlReg + 2) == 65) {
        controlReg[1] = 2;
    }
}

typedef struct RoomParticleRenderRecord {
    void *packet;
    s16 field04;
    s16 field06;
    s16 field08;
    s16 field0A;
    s16 field0C;
    u8 pad0E[2];
    u8 field10;
    u8 field11;
    u8 field12;
    u8 pad13;
    RoomSpriteMatrix matrix;
} RoomParticleRenderRecord;

typedef struct RoomParticleRenderPairState {
    u8 pad00[0x78];
    RoomParticleRenderRecord first;
    RoomParticleRenderRecord second;
} RoomParticleRenderPairState;


void func_800C5538(RoomParticleRenderRecord *record);

void RoomFx_InitParticleRenderPair(
    void *owner, void *unused, RoomParticleRenderPairState *state) {
    RoomSpriteMatrix secondMatrix;
    RoomSpriteMatrix firstMatrix;
    void *callOwner;
    RoomParticleRenderPairState *workState;
    void **root;

    callOwner = owner;
    workState = state;
    root = func_800C2B50();
    firstMatrix = *(RoomSpriteMatrix *)(*(char **)((char *)*root + 0x238));
    secondMatrix = *(RoomSpriteMatrix *)(*(char **)((char *)*root + 0x238));

    firstMatrix.t[0] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x134);
    firstMatrix.t[1] = g_RoomFloorY->y;
    firstMatrix.t[2] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x13C);
    secondMatrix.t[0] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x1B4);
    secondMatrix.t[1] = g_RoomFloorY->y;
    secondMatrix.t[2] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x1BC);

    workState->second.field04 = 10;
    workState->second.packet = g_RoomMotionTrailB;
    workState->second.field06 = 0;
    workState->second.field08 = 400;
    workState->second.field0A = 300;
    workState->second.field0C = 2;
    workState->second.matrix = secondMatrix;
    workState->second.field10 = 32;
    workState->second.field11 = 1;
    workState->second.field12 = 1;
    func_800C5538(&workState->second);

    workState->first.field04 = 10;
    workState->first.packet = g_RoomMotionTrailA;
    workState->first.field06 = 0;
    workState->first.field08 = 400;
    workState->first.field0A = 300;
    workState->first.field0C = 2;
    workState->first.matrix = firstMatrix;
    workState->first.field10 = 32;
    workState->first.field11 = 1;
    workState->first.field12 = 1;
    func_800C5538(&workState->first);

    func_800C66C8(callOwner, 0x576, &firstMatrix);
}

extern void func_800C5A40(void *arg0);

void RoomFx_DrawParticleRenderPair(void *arg0, void *arg1, char *sys) {
    char *sysp = sys;
    char *root;

    root = func_800C2B50();
    func_800C2EAC((u8)root[0x6C]);
    func_800C2FF0(0x10, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C5A40(sysp + 0xAC);
    func_800C5A40(sysp + 0x78);
}

extern int func_800C5EB0(void *obj, s16 *values, int *result);

void RoomFx_UpdateParticleRenderPair(void *arg0, char *state, char *sys) {
    char *statep = state;
    char *sysp = sys;
    int *resultp;
    char *root;
    RoomSharedGlobalState *global;
    int value0;
    int value1;
    int value2;
    s16 values[3];
    int result;
    int ret;

    root = func_800C2B50();
    resultp = &result;
    global = (RoomSharedGlobalState *)D_8009D254;
    value0 = global->field2A;
    values[0] = value0;
    value1 = global->field2A;
    values[1] = value1;
    value2 = global->field2A;
    values[2] = value2;

    func_800C5EB0(sysp + 0xAC, values, resultp);
    ret = func_800C5EB0(sysp + 0x78, values, resultp);

    if (result == 1) {
        *(s16 *)(root + 0x68) = 1;
    }

    if (ret == 1) {
        statep[1] = 2;
    }
}
