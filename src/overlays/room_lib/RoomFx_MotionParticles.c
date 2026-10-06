/*
 * The motion particle set, part 1 of 3: the setup, the timer and anchor handlers, the
 * anchored sprite quad and pair with their step handlers, and the particle
 * seeding.
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
#include "common.h"
#include "room_lib.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "RoomLib_RenderLayouts.h"
#include "RoomLib_DrawMotion.h"
#include "RoomSharedGlobalState.h"
#include "pe1/room_floor.h"


static const RoomFxSeed8 s_MotionQuadSeed = { { 0, 4, 0, 0, 0, 0, 0, 0 } };

/* Motion particle setup: the two trails of ten particles and the four
 * glyphs. */
void RoomFx_MotionParticlesInit(RoomMotionInitOwner *owner, void *unused, RoomMotionInitState *state) {
    RoomMotionInitLink *link;
    RoomMotionParticle *particle;
    unsigned int i;

    func_800C2B40(state);
    state->allocation = func_8006DC18(12);
    link = owner->link;
    state->link = link;
    state->base = link->model->base;
    state->primary = state->link->model->primary;
    state->secondary = state->link->model->secondary;
    state->timer = 30;
    state->field66 = 0;
    state->field68 = 0;

    g_RoomMotionGlyphs[1].size = 44;
    g_RoomMotionGlyphs[1].visible = 1;
    g_RoomMotionGlyphs[1].x = -30;
    g_RoomMotionGlyphs[1].y = 0;
    g_RoomMotionGlyphs[1].red = 128;
    g_RoomMotionGlyphs[1].green = 128;
    g_RoomMotionGlyphs[1].blue = 128;
    g_RoomMotionGlyphs[1].flags = 0;

    g_RoomMotionGlyphs[2].size = 44;
    g_RoomMotionGlyphs[2].visible = 1;
    g_RoomMotionGlyphs[2].x = 0;
    g_RoomMotionGlyphs[2].y = 32;
    g_RoomMotionGlyphs[2].red = 128;
    g_RoomMotionGlyphs[2].green = 128;
    g_RoomMotionGlyphs[2].blue = 128;
    g_RoomMotionGlyphs[2].flags = 0;

    g_RoomMotionGlyphs[0].size = 44;
    g_RoomMotionGlyphs[0].visible = 1;
    g_RoomMotionGlyphs[0].x = -30;
    g_RoomMotionGlyphs[0].y = 128;
    g_RoomMotionGlyph.x = -31;
    g_RoomMotionGlyphs[3].size = 32;
    g_RoomMotionGlyphs[0].red = 128;
    g_RoomMotionGlyphs[0].green = 128;
    g_RoomMotionGlyphs[0].blue = 128;
    g_RoomMotionGlyphs[0].flags = 0;

    g_RoomMotionGlyph.size = 0;
    g_RoomMotionGlyph.visible = 1;
    g_RoomMotionGlyph.y = 128;
    g_RoomMotionGlyph.red = 128;
    g_RoomMotionGlyph.green = 128;
    g_RoomMotionGlyph.blue = 128;
    g_RoomMotionGlyph.flags = 0;

    g_RoomMotionGlyphs[3].visible = 1;
    g_RoomMotionGlyphs[3].x = 100;
    g_RoomMotionGlyphs[3].y = 128;
    g_RoomMotionGlyphs[3].red = 128;
    g_RoomMotionGlyphs[3].green = 128;
    g_RoomMotionGlyphs[3].blue = 128;
    g_RoomMotionGlyphs[3].flags = 0;

    for (i = 0; i < 10; i++) {
        particle = &g_RoomMotionTrailA[i];
        particle->mode = 4;
        particle->alpha = 128;
        particle->kind = 4;
        particle->life = 30;
        particle->speed = 0;
        if (i == 0) particle->drift = -64;
        else particle->drift = 0;
        particle->red = 255;
        particle->green = 255;
        particle->blue = 255;
        particle = &g_RoomMotionTrailB[i];
        particle->mode = 4;
        particle->alpha = 128;
        particle->kind = 4;
        particle->life = 30;
        particle->speed = 0;
        if (i == 0) particle->drift = 64;
        else particle->drift = 0;
        particle->red = 255;
        particle->green = 255;
        particle->blue = 255;
    }
}

void RoomFx_MotionParticlesNop(void) {
}

extern int func_800C2B68(void);

void RoomFx_MotionParticlesTick(int arg0, char *arg1, char *arg2) {
    short *state = (short *)arg2;

    if (state[0x33] != 0) {
        state[0x33]--;
    }

    if (state[0x34] == 1) {
        int timer = state[0x33];

        state[0x34] = 0;
        if (timer == 0) {
            state[0x33] = *(unsigned short *)(arg2 + 0x64);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}

void RoomFx_MotionParticlesAnchor(int a0, int a1, short *out) {
    char *ctx = (char *)func_800C2B50();

    out[0] = RW32(ctx, 0x58);
    out[1] = RW32(ctx, 0x5C);
    out[2] = RW32(ctx, 0x60);
    out[4] = RW32(ctx, 0x38);
    out[5] = RW32(ctx, 0x3C);
    out[6] = RW32(ctx, 0x40);
    out[9] = 0;
    out[8] = 0x400;
}

void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, s32 mode);


typedef struct RoomLibSpriteQuadStack {
    RoomSpriteMatrix scaleMatrix;
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomLibSpriteQuadStack;

/* Builds a uniformly scaled rotation from the shared seed and draws the
 * sprite twice at each of two map anchors: once unrotated at the anchor
 * height and once rotated on the shared floor height. Retail marks this
 * function handwritten; only its COP2 windows remain ASM. Like
 * RoomLib_DrawAnchoredSpritePair.inc, the four pointers are pinned to
 * s0..s3 and the seed argument is pinned behind one empty barrier because
 * the GTE operands give the matrix pointer the most references, which
 * otherwise hands it s0 and hoists its address above the seed load. */
void RoomFx_DrawAnchoredSpriteQuad(void *unused0, void *unused1,
                                            char *params) {
    RoomLibSpriteQuadStack stack;
    /* Retail keeps these in s0..s3; fx is reused for the packet pointers. */
    register char *fx asm("$16");
    char *map;
    RoomSpriteMatrix *scaleMatrix;
    register RoomSpriteMatrix *matrix asm("$19");
    char *root;
    short *floorY;
    int sprite;

    fx = params;
    root = func_800C2B50();
    stack.seed = s_MotionQuadSeed;
    map = *(char **)(*(char **)root + 0x238);
    func_800C2EAC(*(u8 *)(root + 0x6C));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    stack.scaleMatrix.m[2][2] = 0x1000;
    stack.scaleMatrix.m[1][1] = 0x1000;
    stack.scaleMatrix.m[0][0] = 0x1000;
    stack.scaleMatrix.t[2] = 0;
    stack.scaleMatrix.t[1] = 0;
    stack.scaleMatrix.t[0] = 0;
    stack.scaleMatrix.m[2][1] = 0;
    stack.scaleMatrix.m[2][0] = 0;
    stack.scaleMatrix.m[1][2] = 0;
    stack.scaleMatrix.m[1][0] = 0;
    stack.scaleMatrix.m[0][2] = 0;
    stack.scaleMatrix.m[0][1] = 0;
    func_80071A44(&stack.sourceScale, 0, 0x10);

    stack.sourceScale.x = *(s16 *)(fx + 0x10);
    stack.sourceScale.y = *(s16 *)(fx + 0x10);
    stack.sourceScale.z = *(s16 *)(fx + 0x10);
    stack.scale = stack.sourceScale;
    ScaleMatrix(&stack.scaleMatrix, &stack.scale);
    {
        register RoomFxSeed8 *seedArg asm("$4") = &stack.seed;
        asm("" : : "r"(seedArg));
        matrix = &stack.matrix;
        RotMatrix(seedArg, matrix);
    }

    scaleMatrix = &stack.scaleMatrix;
    gte_ldrotmatrix(scaleMatrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][0]);
    gte_stir123_column(matrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][1]);
    gte_stir123_column_at(&stack.matrix.m[0][1]);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][2]);
    gte_stir123_column_at(&stack.matrix.m[0][2]);
    gte_ldtransmatrix(scaleMatrix);
    {
        unsigned short *translation = (unsigned short *)stack.matrix.t;
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        gte_swc2_9_0(translation);
        gte_swc2_10_4(translation);
        gte_swc2_11_8(translation);
    }

    /* Retail reuses the params register for the packet pointers. */
    sprite = *(u16 *)(fx + 0x12);
    fx = (char *)&g_RoomMotionQuadDepth;
    *(s16 *)fx = sprite;
    stack.scaleMatrix.t[0] = *(int *)(map + 0x134);
    stack.scaleMatrix.t[1] = *(int *)(map + 0x138);
    fx -= 10;
    stack.scaleMatrix.t[2] = *(int *)(map + 0x13C);
    func_800C42A4(fx, scaleMatrix, 1);
    stack.scaleMatrix.t[0] = *(int *)(map + 0x1B4);
    stack.scaleMatrix.t[1] = *(int *)(map + 0x1B8);
    stack.scaleMatrix.t[2] = *(int *)(map + 0x1BC);
    func_800C42A4(fx, scaleMatrix, 1);

    fx = (char *)g_RoomMotionQuadPacket;
    floorY = &g_RoomFloorY->y;
    stack.matrix.t[0] = *(int *)(map + 0x134);
    stack.matrix.t[1] = *floorY;
    stack.matrix.t[2] = *(int *)(map + 0x13C);
    func_800C42A4(fx, matrix, 1);
    stack.matrix.t[0] = *(int *)(map + 0x1B4);
    stack.matrix.t[1] = *floorY;
    stack.matrix.t[2] = *(int *)(map + 0x1BC);
    func_800C42A4(fx, matrix, 1);
}

void RoomFx_UpdateTimedFade(void *arg0, char *state, char *obj) {
    char *p = obj;
    if ((unsigned short)(RWU16(state, 2) - 1) < 7U) {
        RWU16(obj, 0x12) += 0x10;
    }
    if ((unsigned short)(RWU16(state, 2) - 0x15) < 0x1DU) {
        RWU16(obj, 0x10) += 0x28;
    }
    if (RW16(state, 2) >= 0x33 && RW16(p, 0x12) >= 0x11) {
        RWU16(p, 0x12) -= 0x10;
    }
    if (RW16(state, 2) == 0x3A) {
        RW8(state, 1) = 2;
    }
}

extern void func_800C4E50(void *arg0);

void RoomFx_InitSpritePair(void *arg0, void *arg1, char *obj) {
    int c80 = 0x80;
    int v = 0x10;

    RW16(obj, 0x120) = v;
    v = 0xC8;
    RW16(obj, 0x12) = c80;
    RW16(obj, 0x128) = c80;
    c80 = 0xFF;
    RW16(obj, 0x126) = v;
    v = 0x40;
    RW8(obj, 0x118) = v;
    v = 0x80;
    RW8(obj, 0x119) = v;
    v = 0x4B0;
    RW16(obj, 0x122) = v;
    v = (int)(obj + 0x14);
    RW16(obj, 0x10) = 0;
    RW8(obj, 0x11C) = c80;
    RW8(obj, 0x11D) = c80;
    RW8(obj, 0x11E) = c80;
    RW8(obj, 0x11A) = c80;
    RW16(obj, 0x124) = 0;
    RW32(obj, 0x114) = v;
    func_800C4E50(obj + 0x114);
}

void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_800C4FC4(void *state, RoomSpriteMatrix *matrix, int mode);


/* Builds a uniformly scaled rotation from the shared seed and draws the
 * sprite at two map anchors. Retail marks this function handwritten; only
 * its COP2 windows remain ASM. */
void RoomFx_DrawAnchoredSpritePair(void *unused0, void *unused1,
                                            char *params) {
    RoomLibSpriteTransformStack stack;
    /* Retail keeps these in s0..s3; the later locals reuse the same slots. */
    RoomSpriteMatrix *scaleMatrix;
    register RoomSpriteMatrix *matrix asm("$17");
    char *fx;
    char *root;
    char *map;
    short *floorY;
    char *renderState;
    int firstY;
    int firstX;
    int secondX;
    int secondY;

    fx = params;
    root = func_800C2B50();
    stack.seed = s_MotionQuadSeed;

    stack.scaleMatrix.m[2][2] = 0x1000;
    stack.scaleMatrix.m[1][1] = 0x1000;
    stack.scaleMatrix.m[0][0] = 0x1000;
    stack.scaleMatrix.t[2] = 0;
    stack.scaleMatrix.t[1] = 0;
    stack.scaleMatrix.t[0] = 0;
    stack.scaleMatrix.m[2][1] = 0;
    stack.scaleMatrix.m[2][0] = 0;
    stack.scaleMatrix.m[1][2] = 0;
    stack.scaleMatrix.m[1][0] = 0;
    stack.scaleMatrix.m[0][2] = 0;
    stack.scaleMatrix.m[0][1] = 0;
    func_80071A44(&stack.sourceScale, 0, 0x10);

    stack.sourceScale.x = *(s16 *)(fx + 0x10);
    stack.sourceScale.y = *(s16 *)(fx + 0x10);
    stack.sourceScale.z = *(s16 *)(fx + 0x10);
    stack.scale = stack.sourceScale;
    scaleMatrix = &stack.scaleMatrix;
    ScaleMatrix(scaleMatrix, &stack.scale);
    {
        register RoomFxSeed8 *seedArg asm("$4") = &stack.seed;
        asm("" : : "r"(seedArg));
        matrix = &stack.matrix;
        RotMatrix(seedArg, matrix);
    }

    gte_ldrotmatrix(scaleMatrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][0]);
    gte_stir123_column(matrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][1]);
    gte_stir123_column_at(&stack.matrix.m[0][1]);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][2]);
    gte_stir123_column_at(&stack.matrix.m[0][2]);
    gte_ldtransmatrix(scaleMatrix);
    {
        unsigned short *translation = (unsigned short *)stack.matrix.t;
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        gte_swc2_9_0(translation);
        gte_swc2_10_4(translation);
        gte_swc2_11_8(translation);
    }

    func_800C3238(2);
    map = *(char **)(*(char **)root + 0x238);
    renderState = fx + 0x114;
    *(s16 *)(fx + 0x128) = *(u16 *)(fx + 0x12);
    /* Both anchors sit on the shared floor height; the first block stores
     * the height before the x coordinate, which fixes retail's register
     * choice. */
    floorY = &g_RoomFloorY->y;
    firstX = *(int *)(map + 0x134);
    firstY = *floorY;
    stack.matrix.t[1] = firstY;
    stack.matrix.t[0] = firstX;
    stack.matrix.t[2] = *(int *)(map + 0x13C);
    func_800C4FC4(renderState, matrix, 1);
    secondX = *(int *)(map + 0x1B4);
    secondY = *floorY;
    stack.matrix.t[0] = secondX;
    stack.matrix.t[1] = secondY;
    stack.matrix.t[2] = *(int *)(map + 0x1BC);
    func_800C4FC4(renderState, matrix, 1);
}

void RoomFx_StepSpritePair(void *arg0, char *state, char *obj) {
    if (RW16(state, 2) < 0x10) {
        RWU16(obj, 0x10) += 0xF0;
        if (RW16(state, 2) < 0x10) {
            RWU16(obj, 0x12) -= 8;
        }
    }
    if (RW16(state, 2) == 0x10) {
        RW8(state, 1) = 2;
    }
}

s32 *func_800C2B10(s32 index);
int func_80071A54(void);

/* Seeds the motion particles: three or six of them depending on the first
 * scene flag, each with a random anchor offset, a random sprite lookup and
 * a random drift that is slower and sideways when the flag is set. */
void RoomFx_SeedMotionParticles(void *unused0, void *unused1,
                                        RoomMotionState *state)
{
    /* Retail reserves one 8-byte local that nothing in the shipped code
     * reads; keep it so the frame matches. */
    s16 unusedVector[4];
    unsigned int i;

    if (*func_800C2B10(1)) {
        state->count = 3;
    } else {
        state->count = 6;
    }
    state->depth = 0;
    i = 0;
    if (state->count != 0) {
        do {
            state->scale[i].x = 0x400;
            state->scale[i].y = 0x400;
            state->particle[i].primaryX = func_80071A54() % 140 - 70;
            state->particle[i].primaryY = func_80071A54() % 140 - 70;
            state->particle[i].primaryZ = func_80071A54() % 140 - 70;
            state->particle[i].secondaryX = func_80071A54() % 140 - 70;
            state->particle[i].secondaryY = func_80071A54() % 140 - 70;
            state->particle[i].secondaryZ = func_80071A54() % 140 - 70;
            state->lookup[i].id = func_80071A54() % 8;
            state->lookup[i].pad = func_80071A54() % 8;
            if (*func_800C2B10(1) == 0) {
                state->velocity[i].primaryX = func_80071A54() % 40 - 20;
                state->velocity[i].primaryY = 0;
                state->velocity[i].primaryZ = func_80071A54() % 40 - 20;
                state->velocity[i].secondaryX = func_80071A54() % 40 - 20;
                state->velocity[i].secondaryY = 0;
                state->velocity[i].secondaryZ = func_80071A54() % 40 - 20;
            } else {
                state->velocity[i].primaryX = func_80071A54() % 26 - 13;
                state->velocity[i].primaryY = -func_80071A54() % 10;
                state->velocity[i].primaryZ = func_80071A54() % 26 - 13;
                state->velocity[i].secondaryX = func_80071A54() % 26 - 13;
                state->velocity[i].secondaryY = -func_80071A54() % 10;
                state->velocity[i].secondaryZ = func_80071A54() % 26 - 13;
            }
            i++;
        } while (i < state->count);
    }
}
