#include "common.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_vector_math.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_sine_table.h"
#include "pe1/gte_types.h"
#include "pe1/player_entity.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_libc.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/room_m350_effects.h"
#include "pe1/room_m350_sweep_trap.h"
#include "pe1/scene_archive.h"
#include "pe1/scene_assets.h"

#include "m350_externs.h"
#include "room_m350_data.h"
#include "room_m350_shared.h"

typedef GteShortVector Particle;

typedef struct { Vector rotation; Vector *position; int color; } PulsingPalettePairParticle;

typedef struct { short delay, count; } State;

typedef struct { Vector *position; short scale, reserved; } RampedPalettePairParticle;

typedef struct { short target, current; } RampedPalettePairState;

typedef RoomM350EffectInstance Instance;

typedef RoomM350EffectActor Actor;

typedef union RoomM350Angles {
    GteShortVector angles;
    GteRotation rotation;
} RoomM350Angles;

typedef union RoomM350Step {
    GteShortVector vector;
    s32 words[2];
} RoomM350Step;

typedef RoomM350SoundOwner RoomM350ControllerOwner;

typedef RoomM350TransformMatrix Transform;

typedef struct RoomM350Instance {
    RoomM350ControllerOwner *owner;
    char reserved04[10];
    unsigned char animation;
    char reserved0F[7];
    unsigned short frame;
    short reserved18;
    unsigned short previousFrame;
    char reserved1C[0x21C];
    Transform *transforms;
} SweepTrapFlowInstance;

typedef RoomM350Actor RoomM350ControllerChannel;

typedef struct { short x,y,z,frame; } FallingTexturePairParticle;

typedef struct { Vector *position; } TripleScalePairParticle;

typedef FieldActorState Owner;

typedef struct { int color; short x, y, z, size; } ModelShellLayersParticle;

typedef RoomM350TableShadeEntry TableEntry;

typedef struct {
    RoomM350SoundOwner *owner;
    char reserved04[10];
    unsigned char id;
    char reserved0F[7];
    unsigned short frame;
    char reserved18[2];
    unsigned short previous;
} Animation;

typedef struct { int reserved[2]; Animation *animation; } AnimationBurstActor;

typedef struct { short remaining, delay; } AnimationBurstState;

typedef struct { Vector *position; short offset,reserved; } PointerQueueParticle;

typedef RoomM350PointerQueue Queue;

typedef struct { Vector position; short delay,reserved; } PositionQueueParticle;

typedef struct { short position[3]; short size; } TableTransformParticle;

typedef GteVector Scale;

typedef struct { void *asset; int yaw; short size,brightness,unknown,turn; unsigned char initialized; } TurningAreaControllerState;

typedef struct { short x, y, z, velocityY, gravity, reserved; } DualBallisticParticle;

int RoomM350_HandSparkEmitter(int event)
{
    Vector offset;
    Matrix matrix;
    Vector vector;
    /* Match note: original purpose of unused sp+0x40..0x47 is unknown. */
    char frameGap[8];
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,8,64,
                             (FieldFxCallbackListCallback)RoomM350_HandSparkCallback);
        case 1:
    {
        int timer=g_FieldFxCurrentAge;
        int count;
        int i;
        Vector *out;
        if(timer>=33) return 2;
        count=(g_RcossinTblCos[((((unsigned int)timer<<7)+2048)&4095)*2]+4096)/2048+1;
        if(count*2<=0) return 0;
        i=0;
        out=&offset;
        do {
            int random;
            int highByte;
            int divisor;
            Vector *base;
            Particle *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if(!particle) return 0;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.z=0;
            RotMatrixYXZ(&vector,&matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x=0; vector.y=0;
            highByte=Inv_ScrambleGrid();
            random=((unsigned int)highByte<<8)|(unsigned int)Inv_ScrambleGrid();
            divisor=192;
            vector.z=random%divisor+64;
            ApplyMatrixSV(&matrix,&vector,out);
            base=&g_RoomM350HandPoints[i&1];
            particle->x=(unsigned short)offset.x+(unsigned short)base->x;
            particle->y=(unsigned short)offset.y+(unsigned short)base->y;
            particle->z=(unsigned short)offset.z+(unsigned short)base->z;
            i++;
        } while(i<count*2);
    }
        break;
    case 2:

    {
        int index=g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter00=16; g_FieldFxSpriteParams.parameter02=1;
        g_FieldFxSpriteParams.extent_x=16; g_FieldFxSpriteParams.extent_y=16;
        g_FieldFxSpriteParams.extent_x=16; g_FieldFxSpriteParams.extent_y=16;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=2; g_FieldFxSpriteParams.parameter06=0;
        g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0; g_FieldFxSpriteParams.tpage=palette;
    }
    }
    return 0;
}

/* The spawner owns the particles rendered by this callback. */

int RoomM350_HandPulseCallback(int event, PulsingPalettePairParticle *effect) {
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (event == 2) {
        int scale = g_RcossinTblCos[((unsigned int)(g_FieldFxCurrentAge - 1) << 8) & 0x1F00] * 4 + 2048;
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        int handle;
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        handle = GetClut(48, palette);
        FieldFx_DrawShapeQuads(effect->position, (struct GteRotation *)effect, scale, scale, 110,
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 5,
            (struct RenderColor *)&effect->color);
    }
    return 0;
}

int RoomM350_HandPulseEmitter(int event, State *state)
{
    switch (event) {
    case 0:

        state->delay = 0;
        state->count = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 16, 18,
                             (FieldFxCallbackListCallback)RoomM350_HandPulseCallback);
        case 1:
    {
        int i;
        if (g_FieldFxCurrentAge >= 17) return 2;
        for (i = 0; i < 2; i++) {
            PulsingPalettePairParticle *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!particle) break;
            particle->rotation.x = (unsigned int)Inv_ScrambleGrid() << 4;
            particle->rotation.y = (unsigned int)Inv_ScrambleGrid() << 4;
            particle->rotation.pad = 1;
            particle->rotation.z = 0;
            particle->position = &g_RoomM350HandPoints[i];
            particle->color = g_RoomM350HandPulseColors[(state->count + i) & 7];
        }
        {
            int count = (unsigned short)state->count;
            state->count = count + 1;
            state->delay = 4 - count;
            if (state->delay <= 0) state->delay = 1;
        }
    }
        break;
    case 2:

    {
        int index = g_FieldFxTpageSlot3;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 32; g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32; g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.extent_x = 32; g_FieldFxSpriteParams.extent_y = 32;
        palette = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 3; g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0; g_FieldFxSpriteParams.depth = 0; g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

/* The spawner owns the scaled particles rendered by this callback. */

int RoomM350_HandGlowCallback(int event, RampedPalettePairParticle *object)
{
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int size = rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 6) & 0xFC0].part.sin * object->scale / 4096;
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        int handle;

        if (kind == 4 && g_FieldFxAltPaletteFlag) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        FieldFx_DrawShapeQuads(object->position, 0, size, size, (s16)g_FieldFxSpriteParams.parameter02 + 216,
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 5,
            &g_RoomM350GlowSpriteColor);
    }
    return 0;
}

int RoomM350_HandGlowEmitter(int event, RampedPalettePairState *state)
{
    switch (event) {
    case 0:

        state->target = 16384;
        state->current = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 6,
                             (FieldFxCallbackListCallback)RoomM350_HandGlowCallback);
        case 1:
    if (!state->target) {
        if (state->current <= 0) return 2;
    } else if (ANIM_OWNER_AS(Actor)->instance->frame >= 41) state->target = 0;
    if (g_FieldFxCurrentAge & 7) return 0;
    if (state->current < state->target) state->current += 4096;
    else if (state->target < state->current) state->current -= 4096;
    if (state->current) {
        int i;
        for (i = 0; i < 2; i++) {
            RampedPalettePairParticle *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!particle) return 0;
            particle->position = &g_RoomM350HandPoints[i];
            particle->scale = state->current;
        }
    }
        break;
    case 2:

    {
        int unit = 16;
        int palette;
                palette = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
        g_FieldFxSpriteParams.palette = 2; g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter00 = unit; g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = unit; g_FieldFxSpriteParams.extent_y = unit;
        g_FieldFxSpriteParams.extent_x = unit; g_FieldFxSpriteParams.extent_y = unit;
        g_FieldFxSpriteParams.parameter0A = 0; g_FieldFxSpriteParams.depth = 0; g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

/* Sweeping beam trap: sprites swept down a column from the actor's head,
 * a flare and two fan blades towards the anchor, then a projected quad test
 * that tags the player once. */
int RoomM350_HandSweepTrapCallback(int mode, RoomM350SweepTrap *trap) {
    GteShortVector position;
    RoomM350Step step;
    RoomM350Angles rotation;
    RoomM350Angles spin;
    GteMatrix matrix;
    GteShortVector quad[4];
    GteMatrix frame;
    int outside;
    int phase;
    RoomM350Trig *trig;
    int intensity;
    int dist;
    int i;
    int k;
    int c;
    int s;
    int dx;
    int dz;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 0x10) return 1;
    } else if (mode == 2) {
        phase = g_FieldFxCurrentAge - 1;
        intensity = phase < 9;
        i = 8;
        if (intensity) i = phase;
        trig = ((RoomM350Trig *)rcossin_tbl);
        rotation.angles.x = -rcossin_tbl[(i * 128) & 0xFFF].bits.sin * 0x260 / 4096 - 0x500;
        rotation.angles.y = ANIM_OWNER_AS(RoomM350TrapChannel)->actor->heading;
        rotation.angles.z = 0x400;
        RotMatrixYXZ(&rotation.angles, &matrix);
        matrix.t[0] = 0;
        matrix.t[1] = 0;
        matrix.t[2] = 0;
        step.words[0] = 0;
        step.vector.z = 0x70;
        gte_ldrotmatrix(&matrix);
        gte_ldtransmatrix(&matrix);
        gte_ldv0(&step.vector);
        gte_rt();
        gte_stsv(&step.vector);
        rotation.angles.pad = 1;
        rotation.angles.x += 0x400;
        position = *trap->anchor;
        if (intensity) intensity = 0x80;
        else intensity = trig[((phase - 8) * 128) & 0xFFF].cos >> 5;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        for (i = phase; position.y < g_RoomFloorY->y; i++) {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_DrawShapeQuads(&position, &rotation.rotation, 0x800, 0x1800,
                          (s16)g_FieldFxSpriteParams.parameter02 * (i & 7) + 0x40,
                          GetClut(0, palette), 1, intensity, &g_RoomM350HandSweepTrapPulseColors[g_FieldFxCurrentAge & 1]);
            position.x += step.vector.x;
            position.y += step.vector.y;
            position.z += step.vector.z;
        }
        spin.angles.x = 0x400;
        spin.angles.y = 0;
        spin.angles.pad = 1;
        spin.angles.z = g_FieldFxCurrentAge << 7;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&position, &spin.rotation, 0x2000, 0x2000,
                          (s16)g_FieldFxSpriteParams.parameter02 * 17 + 0x84,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          1, intensity, &g_RoomM350HandSweepTrapPulseColors[g_FieldFxCurrentAge & 1]);
        }
        dx = position.x - trap->anchor->x;
        dz = position.z - trap->anchor->z;
        dist = Math_IntSqrt(dx * dx + dz * dz);
        rotation.angles.z = 0;
        rotation.angles.x = 0;
        rotation.angles.y -= 0x14;
        for (i = 0; i < 2; i++) {
            FieldFx_DrawRotatedTriangle(&position, &rotation.rotation, dist, 0x200, &g_RoomM350HandSweepTrapTriangleColor, &g_RoomM350BlackColor,
                          &g_RoomM350BlackColor, (s16)intensity, 1);
            rotation.angles.y += 0x28;
        }
        spin.angles.x = 0;
        spin.angles.y = 0;
        spin.angles.pad = 0;
        spin.angles.z = g_FieldFxCurrentAge << 7;
        trig = ((RoomM350Trig *)rcossin_tbl);
        {
            int scale = trig[(phase * 64) & 0xFFF].sin + 0x1000;
            FieldFx_DrawRingBand(trap->anchor, 0x100, 0x1C0, 0x10, &spin.rotation, scale, scale,
                          &g_RoomM350BlackColor, &g_RoomM350HandSweepTrapRingColor,
                      trig[((g_FieldFxCurrentAge - 1) * 64) & 0xFFF].cos >> 5, 1);
        }
        if (trap->done) return 0;
        if (g_PlayerEntity->mode < 4) return 0;
        g_RoomM350HandSweepTrapCorners[2].z = g_RoomM350HandSweepTrapCorners[3].z = -dist;
        c = rcos(ANIM_OWNER_AS(RoomM350TrapChannel)->actor->heading);
        s = rsin(ANIM_OWNER_AS(RoomM350TrapChannel)->actor->heading);
        frame.m[0][2] = s;
        frame.m[2][0] = -s;
        frame.m[1][1] = 0x1000;
        frame.m[0][0] = c;
        frame.m[2][2] = c;
        frame.t[2] = 0;
        frame.t[1] = 0;
        frame.t[0] = 0;
        frame.m[2][1] = 0;
        frame.m[1][2] = 0;
        frame.m[1][0] = 0;
        frame.m[0][1] = 0;
        frame.t[0] = position.x;
        frame.t[1] = position.y;
        frame.t[2] = position.z;
        gte_ldrotmatrix(&frame);
        gte_ldtransmatrix(&frame);
        for (k = 0; k < 4; k++) {
            gte_ldv0(&g_RoomM350HandSweepTrapCorners[k]);
            gte_rt();
            gte_stsv(&quad[k]);
        }
        {
            int player = (((RoomM350Player *)g_PlayerEntity)->z << 16) | (u16)((RoomM350Player *)g_PlayerEntity)->x;
            int prev = (quad[3].z << 16) | (u16)quad[3].x;
            k = 0;
            while (k < 4) {
                int cur = (quad[k].z << 16) | (u16)quad[k].x;
                gte_ldsxy0(player);
                gte_ldsxy2(prev);
                gte_ldsxy1(cur);
                gte_nclip();
                gte_stmac0(&outside);
                if (outside < 0) break;
                k++;
                prev = cur;
            }
        }
        if (k < 4) return 0;
        g_PlayerEntity->state->flags |= 0x4000;
        {
            RoomM350TrapNode *node = ANIM_OWNER_AS(RoomM350TrapChannel)->actor->node;
            GteShortVector unused;
            if (node) node->flags |= 0x80000000;
        }
        trap->done = 1;
        g_RoomM350HandSweepHit = 1;
    }
    return 0;
}
int RoomM350_HandSweepTrapEmitter(int event) {
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList((ANIM_POOL_AS(RoomM350Emitter))->pool,8,2,
                             (FieldFxCallbackListCallback)RoomM350_HandSweepTrapCallback);
        case 1:
    {
        SweepTrapFlowInstance *instance = ANIM_OWNER_AS(RoomM350ControllerChannel)->instance;
        int frame;
        int previous;
        int i;
        if (instance->animation != 11) return 0;
        frame = instance->frame;
        if (frame >= 50) return 2;
        previous = instance->previousFrame;
        if (frame < 8) return 0;
        if (previous < 8) {
            for (i=0;i<2;i++) {
                RoomM350SweepTrap *particle = FieldFx_AcquireCallbackSlot((ANIM_POOL_AS(RoomM350Emitter))->pool);
                if (!particle) break;
                particle->anchor = &g_RoomM350HandPoints[i];
                particle->done = 0;
            }
            {
                SweepTrapFlowInstance *current = ANIM_OWNER_AS(RoomM350ControllerChannel)->instance;
                Asset_Find08w(0x5C6,current->owner->soundMode,
                    (short)current->transforms->position[0],
                    (short)current->transforms->position[1],
                    (short)current->transforms->position[2]);
            }
        }
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00=32;
    g_FieldFxSpriteParams.parameter02=2;
    g_FieldFxSpriteParams.extent_x=32;
    g_FieldFxSpriteParams.extent_y=32;
    g_FieldFxSpriteParams.extent_x=32;
    g_FieldFxSpriteParams.extent_y=32;
    g_FieldFxSpriteParams.parameter0A=0;
    g_FieldFxSpriteParams.depth=0;
    }
    return 0;
}

PE1_STATIC_ASSERT(sizeof(RoomM350SweepTrap) == 8, room_m350_sweep_trap_size);

/* The spawner owns the falling particles updated and rendered by this callback. */

int RoomM350_HandSweepHitCallback(int event, short *position) {
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        position[1] -= position[3]++;
    } else if (event == 2) {
        int kind = g_FieldFxSpriteParams.palette;
        int texture = g_FieldFxPaletteTable[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? texture + 6 : texture + 2);
        FieldFx_DrawShapeQuads((GteShortVector *)position, 0, 8192, 8192,
            (s16)g_FieldFxSpriteParams.parameter02 * (((g_FieldFxCurrentAge - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 6) & 0xFC0].bits.cos >> 5,
            &g_RoomM350HandSweepHitColor);
    }
    return 0;
}

int RoomM350_HandSweepHitEmitter(int event) {
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,8,4,
                             (FieldFxCallbackListCallback)RoomM350_HandSweepHitCallback);
        case 1:
    if(g_RoomM350HandSweepDone) return 2;
    if(g_RoomM350HandSweepHit) {
        FallingTexturePairParticle *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
        RoomM350TransformOwner *player;
        if(!particle) return 0;
        {
            player=(RoomM350TransformOwner *)g_PlayerEntity;
            particle->x=player->transforms->position[0];
            particle->y=player->transforms->position[1];
            particle->z=player->transforms->position[2];
            particle->frame=0;
            asm volatile("" : : : "memory");
        }
        g_RoomM350HandSweepHit=0;
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 32;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 1;
    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.depth = 0;
    }
    return 0;
}

/* The controller owns the position pointers rendered by this callback. */

int RoomM350_HandFlashCallback(int event, void **object)
{
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int sample = rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 6) & 0xFC0].part.sin;
        int kind = g_FieldFxSpriteParams.palette;
        int size = sample * 3;
        int palette = g_FieldFxPaletteTable[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8 : palette + 4);
        FieldFx_DrawShapeQuads((GteShortVector *)*object, 0, size, size, (s16)g_FieldFxSpriteParams.parameter02 * 6 + 128,
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 5, 0);
    }
    return 0;
}

int RoomM350_HandFlashEmitter(int event) {
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,4,2,
                             (FieldFxCallbackListCallback)RoomM350_HandFlashCallback);
        case 1:
    {
        Instance *instance = ANIM_OWNER_AS(Actor)->instance;
        int frame, previous, i;
        if (instance->animation != 11) return 0;
        frame = instance->frame;
        previous = instance->previousFrame;
        if (frame >= 5 && previous < 5) {
            for (i=0;i<2;i++) {
                TripleScalePairParticle *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
                if (!particle) break;
                particle->position = &g_RoomM350HandPoints[i];
            }
            return 0;
        }
        if (frame >= 41) return 2;
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 32;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 1;
    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.depth = 0;
    }
    return 0;
}

int RoomM350_HandSweepController(int event)
{
    register FieldActor *instance asm("$16")=(FieldActor *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    Vector offset;
    if(event==0) {
        Owner *owner=instance->state;
        unsigned int flags=owner->core_flags;
        owner->core_flags=flags|0x40000000;
        asm volatile("" ::: "memory");
        g_RoomM350HandSweepRepeats=3; g_RoomM350HandSweepDone=0; g_RoomM350HandSweepHit=0;
        asm volatile("" ::: "$2");
        return 0;
    }
    if(event==1) {
        short i;
        Vector *input;
        Vector *base;
        int frame;
        short *repeat;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        i=0;
        input=&offset;
        base=g_RoomM350HandPoints;
        offset.x=-112; offset.y=0; offset.z=-80;
        for(;i<2;i++) {
            register RenderMatrix *matrix asm("$2")=g_RoomM350HandActor->render_object.matrices;
            Vector *out;
            if(i) matrix+=15; else matrix+=11;
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                const GteMatrixWords *words=(const GteMatrixWords *)matrix;
                /* Field-specific barriers preserve retail load scheduling around
                 * the individual GTE transfers; they are not hardware hazards. */
                x=words->r11_r12;
                y=words->r13_r21;
                gte_ctc2_0(x);
                gte_ctc2_1(y);
                x=words->r22_r23;
                y=words->r31_r32;
                z=words->r33_pad;
                gte_ctc2_2(x);
                gte_ctc2_3(y);
                asm volatile("" : : "m"(words->r33_pad));
                gte_ctc2_4(z);
                asm volatile("" : : "m"(words->tx));
                x=words->tx;
                asm volatile("" : : "m"(words->tx));
                y=words->ty;
                asm volatile("" : : "m"(words->ty));
                gte_ctc2_5(x);
                asm volatile("" : : "m"(words->tz));
                z=words->tz;
                asm volatile("" : : "m"(words->tz));
                gte_ctc2_6(y);
                asm volatile("" : : "m"(words->tz));
                gte_ctc2_7(z);
                asm volatile("" : : "m"(words->tz));
            }
            gte_ldv0(input); gte_rtv0tr_mac();
            out=(Vector *)((i*8)+(unsigned int)base);
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                out->x=x; out->y=y; out->z=z;
            }
            offset.x=0u-(unsigned short)offset.x;
            g_RoomM350HandPointPadView[i*4]=i;
        }
        frame=instance->anim.parts.integer;
        if(instance->mode!=11) return 0;
        repeat=&g_RoomM350HandSweepRepeats;
        if(*repeat>0) {
            int speed,bias,angle;
            int signedFrame=(short)frame;
            if(signedFrame>=40) {
                instance->anim.fixed=0x30000;
                *repeat=*(volatile unsigned short *)repeat-1;
            }
            if(signedFrame<28) return 0;
            speed=rcossin_tbl[(((signedFrame-28)*170)&0xFFE)].part.sin/32;
            bias=-128;
            if(*(unsigned short *)repeat&1) bias=128;
            angle=FieldFx_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation,(Vec3Fixed *)instance->render_object.model_matrix.translation);
            instance->rot_y=FieldFx_TurnToward(instance->rot_y,(short)(bias+angle),speed);
        } else if((short)frame>=instance->action-1) {
            register Owner *owner asm("$4")=instance->state;
            if(owner) owner->action_state[0]=4;
            g_RoomM350HandSweepDone=1;
            return 1;
        }
    }
    return 0;
}

int RoomM350_OrbHandSparkCallback(int event, ModelShellLayersParticle *particle)
{
    char frameGap[8];
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 8) return 1;
    } else if (event == 2) {
        int size = rcossin_tbl[(((unsigned int)g_FieldFxCurrentAge - 1) << 7) & 0xF80].part.sin * particle->size / 4096;
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        unsigned short clut;
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(48, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)&particle->x, 0, size, size, (s16)g_FieldFxSpriteParams.parameter02 * 5 + 216, clut, 1,
            rcossin_tbl[(((unsigned int)g_FieldFxCurrentAge - 1) << 7) & 0xF80].bits.cos >> 5, (struct RenderColor *)particle);
        FieldFx_DrawStarFan((GteShortVector *)&particle->x, 96, 96, 8, 0, size, size, (struct RenderColor *)g_RoomM350OrbHandSparkColor, &g_RoomM350BlackColor,
            (short)rcossin_tbl[(((unsigned int)g_FieldFxCurrentAge - 1) << 8) & 0xF00].word >> 5, 1);
    }
    return 0;
}

int RoomM350_OrbHandSparkEmitter(int event)
{
    Vector offset;
    Matrix matrix;
    Vector vector;
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 12, 64,
                             (FieldFxCallbackListCallback)RoomM350_OrbHandSparkCallback);
        case 1:
    {
        int i = 0;
        Vector *out;
        if (g_FieldFxCurrentAge >= 41) return 2;
        out = &offset;
        for (; i < 2; i++) {
            int random;
            int highByte;
            int r, g, b, divisor;
            int j, *position;
            ModelShellLayersParticle *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!particle) return 0;
            vector.x = (unsigned int)Inv_ScrambleGrid() << 4;
            vector.y = (unsigned int)Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x = 0; vector.y = 0;
            highByte = Inv_ScrambleGrid();
            random = ((unsigned int)highByte << 8) | (unsigned int)Inv_ScrambleGrid();
            divisor = 192;
            vector.z = random % divisor + 128;
            ApplyMatrixSV(&matrix, &vector, out);
            position = g_RoomM350OrbActor->render_object.matrices[i ? 11 : 15].translation;
            for (j = 0; j < 3; j++) (&particle->x)[j] = ((short *)out)[j] + (unsigned int)position[j];
            particle->size = (unsigned int)rcossin_tbl[((int)((unsigned int)g_FieldFxCurrentAge << 11) / 40) & 4095].word + 4096;
            r = Inv_ScrambleGrid(); g = Inv_ScrambleGrid(); b = Inv_ScrambleGrid();
            particle->color = (r >> 1) | ((unsigned int)(g >> 1) << 8) | ((unsigned int)(b >> 1) << 16);
        }
    }
        break;
    case 2:

    {
        int index = g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 16; g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16; g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.extent_x = 16; g_FieldFxSpriteParams.extent_y = 16;
        palette = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 2; g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0; g_FieldFxSpriteParams.depth = 0; g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

int RoomM350_OrbBurstCallback(int event, void **object)
{
    short vector[4];
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 8) {
            return 1;
        }
    } else if (event == 2) {
        GteSineWord *base = rcossin_tbl;
        int counter = g_FieldFxCurrentAge;
        TableEntry *entry = (TableEntry *)&base[((unsigned int)(counter - 1) << 7) & 0xF80];
        int size = entry->size;

        vector[0] = 0;
        vector[1] = 0;
        vector[2] = counter * 128;
        vector[3] = 0;
        FieldFx_DrawStarFan(*object, 320, 320, 16, (struct GteRotation *)vector, size, size,
            (struct RenderColor *)g_RoomM350OrbBurstRingColorA, &g_RoomM350BlackColor, entry->shade >> 5, 1);
        {
            int nextCounter = g_FieldFxCurrentAge;
            size = nextCounter * 512 + 2048;
            FieldFx_DrawRingBand(*object, 256, 512, 16, (struct GteRotation *)vector, size, size,
                &g_RoomM350BlackColor, (struct RenderColor *)g_RoomM350OrbBurstRingColorB,
                (short)base[((unsigned int)(nextCounter - 1) << 8) & 0xF00].word >> 5, 1);
        }
    }
    return 0;
}

int RoomM350_OrbBurstEmitter(int event, AnimationBurstState *state)
{
    switch (event) {
    case 0:
        state->remaining = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 4, 4,
                             (FieldFxCallbackListCallback)RoomM350_OrbBurstCallback);
    case 1:
        {
            Animation *animation = ANIM_OWNER_AS(AnimationBurstActor)->animation;
            if (animation->id == 13) {
                int frame = animation->frame;
                if (frame >= 26) {
                    return 2;
                }
                {
                    int previous = animation->previous;
                    if (frame >= 8 && previous < 8) {
                        state->remaining = 4;
                        state->delay = 0;
                    }
                }
            }
            if (state->remaining != 0 && state->delay-- <= 0) {
                short **effect = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
                if (effect == 0) {
                    return 0;
                }
                *effect = &g_RoomM350OrbSpawnPoint.x;
                state->delay = 1;
                state->remaining--;
                Asset_Find08w(1479, ANIM_OWNER_AS(AnimationBurstActor)->animation->owner->soundMode,
                    (*effect)[0], (*effect)[1], (*effect)[2]);
            }
        }
        break;
    case 2:
        break;
    }
    return 0;
}

/* Moving effect with collision, queued trail positions and ground rendering. */
int RoomM350_OrbCallback(int event,RoomM350Particle *p)
{
    GteMatrix matrix;
    GteShortVector offset,ground;
    /* Match note: target saves registers 16 bytes later; original local unknown. */
    char frameGap[16];
    if(event==1) {
        int timer=g_FieldFxCurrentAge;
        int brightness;
        if(timer>=48) {
            int result=1;
            g_RoomM350OrbState.activeCount=(unsigned short)g_RoomM350OrbState.activeCount-1;
            return result;
        }
        if(p->tail.value.active.frame>=48) return 0;
        if(!p->tail.value.active.hit) {
            if(p->motion.value.rotation.x>0) p->motion.value.rotation.x-=64;
            else {
                GteSineWord *trig=rcossin_tbl;
                int speed=trig[(timer<<4)&0xFF0].part.sin/4;
                int angle=FieldFx_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation,(Vec3Fixed *)ANIM_OWNER_AS(RoomM350EffectActor)->instance->transform.t);
                int yaw=FieldFx_TurnToward(p->motion.value.rotation.y,(short)angle,speed);
                int sum,product;
                p->motion.value.rotation.y=yaw;
                product=trig[(g_FieldFxCurrentAge+2048)&4095].part.sin*160;
                sum=yaw+80;
                p->motion.value.rotation.y=sum-product/4096;
            }
            RotMatrixYXZ(&p->motion.value.rotation,&matrix);
            matrix.t[0]=p->position.x;
            matrix.t[1]=p->position.y;
            matrix.t[2]=p->position.z;
            offset.x=0; offset.y=0; offset.z=0u-(unsigned short)p->tail.value.active.speed;
            gte_ldrotmatrix(&matrix); gte_ldtransmatrix(&matrix);
            gte_ldv0(&offset); gte_rtv0tr_mac();
            gte_stsv(&p->position);
        }
        if(p->tail.value.active.frame<40) brightness=((short)rcossin_tbl[(p->tail.value.active.frame<<9)&0xE00].word>>6)+128;
        else {
            unsigned int value=rcossin_tbl[((p->tail.value.active.frame-40)<<7)&0xF80].part.cos;
            brightness=value>>5;
        }
        p->tail.value.active.brightness=brightness;
        if(!p->tail.value.active.hit && g_PlayerEntity->mode>=4) {
            int dx=g_PlayerEntity->render_object.model_matrix.translation[0]-p->position.x;
            int dz=g_PlayerEntity->render_object.model_matrix.translation[2]-p->position.z;
            if(Math_IntSqrt((unsigned int)dx*dx+(unsigned int)dz*dz)<128) {
                short n;
                p->tail.value.active.hit=1;
                ((RoomM350EffectInstance *)g_PlayerEntity)->owner->status|=0x4000;
                if(ANIM_OWNER_AS(RoomM350EffectActor)->instance->owner) ANIM_OWNER_AS(RoomM350EffectActor)->instance->owner->flags|=0x80000000;
                p->tail.value.active.speed=0;
                if(p->tail.value.active.frame<40) p->tail.value.active.frame=40;
                n=g_RoomM350OrbState.pointerCount;
                if(n<4) {
                    int next=n;
                    asm("" : "=r"(next) : "0"(next));
                    g_RoomM350OrbState.pointerCount=next+1;
                    {
                        RoomM350PointerQueue *queue=(RoomM350PointerQueue *)((char *)&g_RoomM350OrbState.pointerCount-PE1_OFFSETOF(RoomM350PointerQueue,count));
                        queue->positions[n]=&p->position;
                    }
                    asm volatile("" : : "r"(next));
                }
            }
        }
        if(p->tail.value.active.frame>=36 && !(p->tail.value.active.frame&3)) {
            short n=g_RoomM350OrbState.pointerCount;
            if(n<4) {
                int next=n;
                asm("" : "=r"(next) : "0"(next));
                g_RoomM350OrbState.pointerCount=next+1;
                {
                    RoomM350PointerQueue *queue=(RoomM350PointerQueue *)((char *)&g_RoomM350OrbState.pointerCount-PE1_OFFSETOF(RoomM350PointerQueue,count));
                    queue->positions[n]=&p->position;
                }
                asm volatile("" : : "r"(next));
            }
        }
        {
            unsigned short n=g_RoomM350OrbState.positionCount;
            int next=n+1;
            int shifted=(unsigned int)n<<16;
            g_RoomM350OrbState.positionCount=next;
            {
                RoomM350PositionQueue *queue=(RoomM350PositionQueue *)((char *)&g_RoomM350OrbState.positionCount-PE1_OFFSETOF(RoomM350PositionQueue,count));
                queue->positions[shifted>>16]=p->position;
            }
        }
        p->tail.value.active.frame=(unsigned short)p->tail.value.active.frame+1;
    } else if(event==2) {
        int clut,palette;
        if(p->tail.value.active.frame>=48) return 0;
        palette=g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if(g_FieldFxSpriteParams.palette==4 && g_FieldFxAltPaletteFlag) palette+=4;
        clut=GetClut(32,palette);
        FieldFx_DrawShapeQuads(&p->position,0,4096,4096,108,(unsigned short)clut,1,p->tail.value.active.brightness,0);
        ground.x=p->position.x; ground.y=g_RoomFloorY->raw; ground.z=p->position.z;
        FieldFx_DrawStarFan(&ground,192,192,8,&g_RoomM350StarFanTilt,4096,4096,&g_RoomM350OrbGroundFanColor,&g_RoomM350BlackColor,p->tail.value.active.brightness,1);
    }
    return 0;
}
int RoomM350_OrbEmitter(int event) {
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,28,3,
                             (FieldFxCallbackListCallback)RoomM350_OrbCallback);
        case 1:
    {
        RoomM350EffectInstance *instance;
        RoomM350Particle *particle;
        int frame, previous;
        if (g_RoomM350OrbState.completionFlags[0]) return 2;
        instance=ANIM_OWNER_AS(RoomM350EffectActor)->instance;
        if (instance->animation != 13) return 0;
        frame=instance->frame;
        previous=instance->previousFrame;
        if (frame<15 || previous>=15) return 0;
        particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        particle->position=g_RoomM350OrbSpawnPoint;
        particle->position.pad=16;
        particle->motion.value.spawn.speed=512;
        {
            int yaw=ANIM_OWNER_AS(RoomM350EffectActor)->instance->yaw;
            particle->motion.value.spawn.phase=0;
            particle->motion.value.spawn.yaw=yaw;
        }
        {
            int count=g_RoomM350OrbState.activeCount;
            particle->tail.value.spawn.duration=16;
            particle->tail.value.spawn.frame=0;
            particle->tail.value.spawn.state=0;
            particle->tail.value.spawn.size=100-count*20;
        }
        g_RoomM350OrbState.activeCount++;
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 32;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 0;
    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.depth = 64;
    }
    return 0;
}

int RoomM350_OrbImpactCallback(int event, PointerQueueParticle *object)
{
    Vector position;
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        object->offset -= 16;
    } else if (event == 2) {
        int frame,kind,palette,handle;
        position = *object->position;
        position.y += object->offset;
        kind = g_FieldFxSpriteParams.palette;
        frame = g_FieldFxCurrentAge - 1;
        palette = g_FieldFxPaletteTable[kind];
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6 : palette + 2);
        FieldFx_DrawShapeQuads((GteShortVector *)&position, 0, 8192, 8192, (s16)g_FieldFxSpriteParams.parameter02 * (frame >> 1),
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)frame << 7) & 0xF80].word >> 5,
            &g_RoomM350OrbImpactColor);
    }
    return 0;
}

int RoomM350_OrbImpactEmitter(int event) {
    int firstIndex;
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,8,16,
                             (FieldFxCallbackListCallback)RoomM350_OrbImpactCallback);
        case 1:
    if (g_RoomM350OrbState.completionFlags[0] && g_RoomM350OrbState.activeCount==0) return 2;
    {
        short *count=&g_RoomM350OrbState.pointerCount;
        if (g_RoomM350OrbState.pointerCount) {
            int i;
            firstIndex=0;
            i=firstIndex;
            if (g_RoomM350OrbState.pointerCount>firstIndex) {
                Queue *queue=(Queue *)&count[-PE1_OFFSETOF(Queue, count) / 2];
                Vector **position=queue->positions;
                while (i<queue->count) {
                    {
                        PointerQueueParticle *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->offset=0;
                    }
                    i++;
                    position++;
                }
            }
            g_RoomM350OrbState.pointerCount=0;
        }
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 32;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 1;
    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.depth = 0;
    }
    return 0;
}

int RoomM350_OrbTrailCallback(int event, PositionQueueParticle *object)
{
    short vector[4];
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (event == 2) {
        int frame = g_FieldFxCurrentAge - 2;
        if (frame >= 0) {
            int size,kind,palette,handle;
            vector[0] = 1024; vector[1] = object->position.pad;
            vector[2] = 1024; vector[3] = 1;
            size = g_RcossinTblCos[((unsigned int)frame << 7) & 0x1F80];
            kind = g_FieldFxSpriteParams.palette; palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            handle = GetClut(0, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)object, (struct GteRotation *)vector, size, size * 2, (s16)g_FieldFxSpriteParams.parameter02 * object->delay + 64,
                (unsigned short)handle, 1,
                rcossin_tbl[((unsigned int)frame << 6) & 0xFC0].bits.cos >> 5,
                &g_RoomM350GlowSpriteColor);
        }
    }
    return 0;
}

int RoomM350_OrbTrailEmitter(int event) {
    int firstIndex;
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,12,50,
                             (FieldFxCallbackListCallback)RoomM350_OrbTrailCallback);
        case 1:
    if (g_RoomM350OrbState.completionFlags[0] && g_RoomM350OrbState.activeCount==0) return 2;
    {
        short *count=&g_RoomM350OrbState.positionCount;
        if (g_RoomM350OrbState.positionCount) {
            int i;
            firstIndex=0;
            i=firstIndex;
            if (g_RoomM350OrbState.positionCount>firstIndex) {
                RoomM350PositionQueue *queue=(RoomM350PositionQueue *)&count[-PE1_OFFSETOF(RoomM350PositionQueue, count) / 2];
                Vector *position=queue->positions;
                while (i<queue->count) {
                    {
                        PositionQueueParticle *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->position.pad=ANIM_OWNER_AS(Actor)->instance->yaw;
                        particle->delay=g_FieldFxCurrentAge & 7;
                    }
                    i++;
                    position++;
                }
            }
            g_RoomM350OrbState.positionCount=0;
        }
    }
        break;
    case 2:

    {
        int index=g_FieldFxTpageSlot3, palette;
        g_FieldFxSpriteParams.parameter00=32; g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32; g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=3; g_FieldFxSpriteParams.parameter06=0; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0; g_FieldFxSpriteParams.tpage=palette;
    }
    }
    return 0;
}

int RoomM350_OrbTableFlashCallback(int event, short *position) {
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 8) return 1;
    } else if (event == 2) {
        int kind = g_FieldFxSpriteParams.palette;
        int texture = g_FieldFxPaletteTable[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? texture + 8 : texture + 4);
        FieldFx_DrawShapeQuads((GteShortVector *)position, 0, position[3], position[3],
            (s16)g_FieldFxSpriteParams.parameter02 * g_RoomM350OrbTableFlashSizes[(g_FieldFxCurrentAge - 1) / 2] + 128,
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 5,
            &g_RoomM350GlowSpriteColor);
    }
    return 0;
}

int RoomM350_OrbTableFlashEmitter(int event) {
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,8,10,
                             (FieldFxCallbackListCallback)RoomM350_OrbTableFlashCallback);
        case 1:
    if(g_RoomM350OrbState.completionFlags[0]) return 2;
    {
        int frame=ANIM_OWNER_AS(Actor)->instance->frame;
        unsigned int relative=frame-7;
        int i;
        if(relative>=6) return 0;
        for(i=0;i<2;i++) {
            Entry *entry=&g_RoomM350OrbTableFlashEntries[frame][i];
            TableTransformParticle *particle;
            Instance *instance;
            int j;
            if(!entry->index) return 0;
            particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if(!particle) return 0;
            if(entry->alternate) instance=(Instance *)g_RoomM350OrbActor;
            else instance=ANIM_OWNER_AS(Actor)->instance;
            {
                int *source=instance->transforms[entry->index].t;
                for(j=0;j<3;j++) particle->position[j]=source[j];
            }
            particle->size=((int *)rcossin_tbl)[(relative<<7)&0xF80]+2048;
        }
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 32;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 1;
    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.depth = 0;
    }
    return 0;
}

int RoomM350_OrbController(int event) {
    FieldActor *instance=(FieldActor *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    Vector offset;
    if(event==0) {
        register int one=1;
        {
            Owner *owner=instance->state;
            register unsigned int flags=owner->core_flags;
            flags |= 0x40000000;
            owner->core_flags=flags;
        }
        one=2;
        g_RoomM350OrbState.repeats=one;
        g_RoomM350OrbState.completionFlags[0]=0; g_RoomM350OrbState.completionFlags[1]=0; g_RoomM350OrbState.completionFlags[2]=0;
        g_RoomM350OrbState.pointerCount=0; g_RoomM350OrbState.positionCount=0; g_RoomM350OrbState.activeCount=0;
        asm volatile("" : : : "$2");
        return 0;
    }
    if(event==1) {
        RenderMatrix *matrix;
        int frame;
        int repetitions;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        matrix=&g_RoomM350OrbActor->render_object.matrices[17];
        offset.x=0; offset.y=-144; offset.z=128;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        gte_stsv(&g_RoomM350OrbSpawnPoint);
        if(instance->mode!=13) return 0;
        repetitions=g_RoomM350OrbState.repeats;
        frame=instance->anim.parts.integer;
        if(repetitions>0) {
            s16 speed;
            int angle;
            frame=(short)frame;
            if(frame>=25) {
                instance->anim.fixed=0x60000;
                g_RoomM350OrbState.repeats=(unsigned short)g_RoomM350OrbState.repeats-1;
            }
            if(frame<19) return 0;
            speed=rcossin_tbl[((frame-19)*341)&4095].part.sin;
            speed/=32;
            angle=FieldFx_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation,(Vec3Fixed *)instance->render_object.model_matrix.translation);
            instance->rot_y=FieldFx_TurnToward(instance->rot_y,(short)angle,speed);
        } else {
            if((short)frame>=instance->action-1) {
                if(instance->state) instance->state->action_state[0]=4;
                g_RoomM350OrbState.completionFlags[0]=1;
                return 1;
            }
        }
    }
    return 0;
}

int RoomM350_AreaSparkCallback(int event, RoomM350RadialSpriteParticle *object)
{
    short position[4];
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int entry = rcossin_tbl[((unsigned int)g_FieldFxCurrentAge << 7) & 0xF80].word;
        int scale = entry >> 16;
        int shade = (short)entry >> 5;
        int *origin = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transforms[33].t;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = object->position[i] * scale / 4096 + origin[i];
        }
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)position, 0, object->size, object->size, (s16)g_FieldFxSpriteParams.parameter02 + 216,
            (unsigned short)handle, 1, shade, (struct RenderColor *)g_RoomM350AreaSparkColor);
    }
    return 0;
}

/* Event state: rotation, emission count, brightness, fade phase and size. */

int RoomM350_AreaSparkEmitter(int event,RoomM350RadialSpriteState *inputState)
{
    RoomM350RadialSpriteState *state=inputState;
    union { GteMatrix matrix; GteShortVector position; } local;
    GteShortVector vector;
    switch (event) {
    case 0:

        {
            RoomM350EffectInstance *instance=ANIM_OWNER_AS(RoomM350EffectActor)->instance;
            int *position=instance->transforms[33].t;
            RoomM350EffectEmitter *emitter;
            Asset_Find08w(0x5C9,(int)instance->owner->asset,(short)position[0],(short)position[1],(short)position[2]);
            *(int *)&state->rotation=0;
            emitter=ANIM_POOL_AS(RoomM350EffectEmitter);
            ((int *)&state->rotation)[1]=0;
            state->count=0; state->fade=0;
            return FieldFx_InitCallbackList(emitter->pool,8,18,
                                 (FieldFxCallbackListCallback)RoomM350_AreaSparkCallback);
        }
        case 1:
    {
        int phase,sample;
        register int i asm("$19");
        if(g_RoomM350AreaDone) {
            unsigned short old=*(volatile unsigned short *)&state->fade;
            phase=(short)old*64+1024;
            state->fade=old+1;
            if(phase>=2048) return 2;
        } else {
            int timer=g_FieldFxCurrentAge;
            phase=timer<32 ? (unsigned int)timer<<5:1024;
        }
        sample=(short)rcossin_tbl[phase&4095].word;
        state->brightness=sample>>5;
        state->size=(unsigned int)sample<<2;
        if(state->count>=16) return 0;
        i=0;
        while(i<2) {
            register RoomM350RadialSpriteParticle *particle asm("$17")=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
            int random;
            int highByte;
            int divisor;
            if(!particle) break;
            i++;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4; vector.z=0;
            RotMatrixYXZ(&vector,&local.matrix);
            asm volatile("" : : "i"(&&radial));
radial:
            vector.x=0; vector.y=0;
            highByte=Inv_ScrambleGrid();
            random=((unsigned int)highByte<<8)|Inv_ScrambleGrid();
            divisor=44;
            vector.z=random%divisor+320;
            ApplyMatrixSV(&local.matrix,&vector,(GteShortVector *)particle);
            particle->size=(unsigned int)state->count*128+2048;
        }
        state->count=(unsigned short)state->count+1;
    }
        break;
    case 2:

    {
        int index=g_FieldFxTpageSlot2;
        int i;
        int palette;
        RoomM350EffectActor *actor;
        register int specialKind asm("$17");
        register unsigned short *palettes asm("$19");
        volatile int *position;
        short *out;
        g_FieldFxSpriteParams.parameter00=16; g_FieldFxSpriteParams.parameter02=1;
        g_FieldFxSpriteParams.extent_x=16; g_FieldFxSpriteParams.extent_y=16; g_FieldFxSpriteParams.extent_x=16; g_FieldFxSpriteParams.extent_y=16;
        palette=g_FieldFxTpageTable[index];

        actor=ANIM_OWNER_AS(RoomM350EffectActor);
        g_FieldFxSpriteParams.palette=2; g_FieldFxSpriteParams.parameter06=0; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0; g_FieldFxSpriteParams.tpage=palette;

        i=0;
        position=actor->instance->transforms[33].t;
        out=&local.position.x;
        for(;i<3;i++) *out++=*position++;
        i=0;
        palettes=g_FieldFxPaletteTable;
        state->rotation.z=(unsigned int)g_FieldFxCurrentAge<<8;
        specialKind=4;
        while(i<2) {
            int kind=g_FieldFxSpriteParams.palette;
            int clut,pal=palettes[kind];
            if(kind==specialKind && g_FieldFxAltPaletteFlag) pal+=4;
            clut=GetClut(32,pal);
            FieldFx_DrawShapeQuads(&local.position,&state->rotation,state->size,state->size,(s16)g_FieldFxSpriteParams.parameter02+216,(unsigned short)clut,1,state->brightness,(struct RenderColor *)g_RoomM350AreaSparkEmitterColor);
            state->rotation.z=0u-(unsigned short)state->rotation.z;
            ++i;
        }
    }
    }
    return 0;
}

int RoomM350_AreaZoneEmitter(int event,TurningAreaControllerState *state)
{
    union { struct { Vector vertices[4]; int area; } collision; Matrix matrix; } local;
    Matrix base;
    Vector position,rotation;
    Scale scale;
    FieldActor *instance=(FieldActor *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    switch (event) {
case 0:
    {
        state->asset=Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0],0xC54E0704);
        FieldFx_ModelScrollUv(state->asset,0,0);
        state->size=0;
        {
            int yaw=(g_RoomM350AreaConfigYaw[0]);
            state->initialized=0;
            state->yaw=yaw<<8;
        }
        return 0;
    }
case 1:
    {
        int timer=g_FieldFxCurrentAge,size;
        register int high asm("$9");
        register int quotient asm("$2");
        int intermediate;
        if(timer<16) size=((4096-rcossin_tbl[((unsigned int)timer<<6)&0xFC0].part.sin)>>1)+2048;
        else if(timer>=91) {
            int sample=rcossin_tbl[((unsigned int)(timer-90)<<6)&0xFC0].part.cos;
            asm volatile("" : "=r"(sample) : "0"(sample));
            size=sample>>1;
        }
        else size=2048;
        /* Expose the high product; the final sign correction still implements /3. */
        intermediate=size/3+(size>>31);
        asm("" : "=h"(intermediate) : "0"(intermediate));
        high=intermediate;
        asm volatile("" : : "r"(high));
        quotient=high-(size>>31);
        state->size=quotient;
        if(state->size<=0) return 1;
        state->brightness=size<=2048 ? size>>4:(4096-size)>>4;
        if(!state->initialized) {
            if(g_RoomM350AreaTrackPlayer) {
                int distance=FieldFx_GetDistanceXZ((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
                int angle=FieldFx_VecToAngle((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
                int yaw=state->yaw;
                int rawDelta=((unsigned int)angle-(yaw>>8))&4095;
                register int delta asm("$4")=rawDelta;
                int speed,factor;
                if(rawDelta<512) { state->yaw=(unsigned int)yaw-0x20000; delta=rawDelta+512; }
                if((short)delta>3584) { delta-=512; state->yaw=(unsigned int)state->yaw+0x20000; }
                if((short)delta>2048) delta=4096-delta;
                speed=(short)distance-512;
                if(speed<0) speed=0;
                factor=1536; if(speed>1024) factor=1024;
                speed=g_RcossinTblCos[((speed/4)&4095)*2]*factor/4096;
                if(speed<0) speed=0;
                speed+=(short)delta;
                asm volatile("" : : "r"(delta));
                if(((short)angle-(state->yaw>>8))&2048) speed=-speed;
                state->turn=((int)((unsigned int)speed<<8))/128;
            } else state->turn=0;
            state->initialized=1;
        }
        {
            int cosine,sine,i;
            Matrix *matrix;
            register int *out asm("$6");
            state->yaw=(unsigned int)state->yaw+state->turn;
            cosine=rcos(state->yaw>>8);
            sine=rsin(state->yaw>>8);
            i=0; matrix=&g_RoomM350AreaMatrix; out=matrix->t;
            matrix->m[0][0]=cosine;
            g_RoomM350AreaMatrix.m[0][2]=sine; g_RoomM350AreaMatrix.m[2][0]=0u-sine; g_RoomM350AreaMatrix.m[2][2]=cosine;
            g_RoomM350AreaMatrix.t[2]=0; g_RoomM350AreaMatrix.t[1]=0; g_RoomM350AreaMatrixTxView=0;
            g_RoomM350AreaMatrix.m[2][1]=0; g_RoomM350AreaMatrix.m[1][2]=0; g_RoomM350AreaMatrix.m[1][0]=0; g_RoomM350AreaMatrix.m[0][1]=0; g_RoomM350AreaMatrix.m[1][1]=4096;
            for(;i<3;i++) *out++=instance->render_object.model_matrix.translation[i];
        }
        if(state->brightness<64) return 0;
        {
            Matrix *matrix=&g_RoomM350AreaMatrix;
            Vector *in;
            Vector *out;
            int i;
            asm("" : : "r"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7", "$8");
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            i=0; out=local.collision.vertices; in=g_RoomM350AreaCorners;
            for(;i<4;i++) {
                gte_ldv0(in);
                gte_rtv0tr_mac();
                gte_stsv(out);
                out++; in++;
            }
        }
        {
            int i,previous,point;
            register Vector *out asm("$6");
            int *area;
            i=0; area=&local.collision.area; out=local.collision.vertices;
            previous=((unsigned int)local.collision.vertices[3].z<<16)|(unsigned short)local.collision.vertices[3].x;
            point=((unsigned int)g_PlayerEntity->render_object.model_matrix.translation[2]<<16)|(unsigned short)g_PlayerEntity->render_object.model_matrix.translation[0];
            for(;i<4;i++) {
                int current=((unsigned int)out->z<<16)|(unsigned short)out->x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(area);
                if(local.collision.area<0) break;
                out++; previous=current;
            }
            if(i>=4) {
                unsigned char *inside=&g_RoomM350AreaPlayerInside;
                if(!*inside) {
                    Owner *owner;
                    g_PlayerEntity->state->flags|=0x4000;
                    owner=instance->state;
                    if(owner) owner->core_flags|=0x80000000;
                    *inside=1; g_RoomM350AreaEntered=1;
                }
            } else g_RoomM350AreaPlayerInside=0;
        }
    }
    break;
case 2:
    {
        int page,palette;
        u16 rawPage;
        int kind;
        int i;
        register int size;
        int *in,*out;
        Matrix *transforms;
        i=0;
        base=g_RoomM350AreaMatrix;
        transforms=(Matrix *)instance->render_object.matrices;
        out=base.t; in=transforms[33].t;
        for(;i<3;i++) *out++=*in++;
        rawPage=GetTPage(0,1,0,0);
        page=(unsigned short)(g_FieldFxTpageTable[g_FieldFxTpageSlot11]|rawPage);
        kind=g_FieldFxSpriteParams.palette;
        palette=g_FieldFxPaletteTable[kind];
        palette=GetClut(0,(kind==4 && g_FieldFxAltPaletteFlag!=0) ? palette+7 : palette+3);
        FieldFx_ModelSetTexture(page,(unsigned short)palette);
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(state->asset);
        FieldFx_ModelScaleColors(state->asset,(unsigned short)state->brightness);
        i=0;
        for (;i<3;) {
            register int next asm("$7");
            register int product asm("$3");
            register int high asm("$9");
            int intermediate;
            register void *clearTarget asm("$4");
            register int clearValue asm("$5");
            register int clearLength asm("$6");
            local.matrix=base;
            next=i+1;
            product=state->size*next;
            intermediate=product/3+(product>>31);
            asm("" : "=h"(intermediate) : "0"(intermediate));
            clearTarget=&scale;
            clearValue=0;
            clearLength=16;
            i=next;
            asm volatile("" : : "r"(i));
            product>>=31;
            asm("" : "=r"(high) : "0"(intermediate));
            size=high-product;
            memset(clearTarget,clearValue,clearLength);
            scale.x=size; scale.y=size; scale.z=2048;
            ScaleMatrix((GteMatrix *)&local.matrix, (const GteVector *)&scale);
            FieldFx_ModelDraw(state->asset,(GteMatrix *)&local.matrix);
        }
        FieldFx_ModelRestoreColors(state->asset);
        i=0;
        position.x=instance->render_object.model_matrix.translation[0]; position.y=instance->render_object.model_matrix.translation[1]; position.z=instance->render_object.model_matrix.translation[2];
        {
            int yaw;
            rotation.x=0;
            yaw=state->yaw;
            rotation.z=0;
            rotation.y=(yaw>>8)+2024;
        }
        {
            register int triple=state->size*3;
            size=triple*2;
            for (;i<3;i++) {
                FieldFx_DrawRotatedTriangle(&position,(GteRotation *)&rotation,size,512,(RenderColor *)g_RoomM350AreaZoneTriangleColor,&g_RoomM350BlackColor,&g_RoomM350BlackColor,state->brightness,1);
                rotation.y=(unsigned short)rotation.y+24;
            }
        }
    }
    }
    return 0;
}

int RoomM350_AreaDropletCallback(int event, short *position) {
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        position[1] += position[3];
        position[3] -= position[4];
    } else if (event == 2) {
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        int handle;
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        handle = GetClut(16, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)position, 0, 12288, 12288,
            (s16)g_FieldFxSpriteParams.parameter02 * (((g_FieldFxCurrentAge - 1) >> 1) & 7) + 200,
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 5,
            (struct RenderColor *)g_RoomM350AreaDropletColor);
    }
    return 0;
}

int RoomM350_AreaDropletEmitter(int event)
{
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 12, 16,
                             (FieldFxCallbackListCallback)RoomM350_AreaDropletCallback);
        case 1:
    if (g_FieldFxCurrentAge >= 107) return 2;
    if (g_FieldFxCurrentAge & 1) {
        Matrix *matrices = ((Instance *)ANIM_OWNER_AS(Actor)->instance)->transforms;
        DualBallisticParticle *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (particle) {
            int random = Inv_ScrambleGrid();
            particle->x = (unsigned int)matrices[33].t[0] - (((unsigned int)random << 1) - 256);
            particle->y = matrices[33].t[1];
            random = Inv_ScrambleGrid();
            particle->z = (unsigned int)matrices[33].t[2] - (((unsigned int)random << 1) - 256);
            particle->velocityY = 0;
            particle->gravity = ((g_FieldFxCurrentAge >> 1) & 1) + 2;
        }
    }
    if (g_RoomM350AreaPlayerInside) {
        Matrix *matrices = ((Instance *)g_PlayerEntity)->transforms;
        DualBallisticParticle *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!particle) return 0;
        {
            int random = Inv_ScrambleGrid();
            particle->x = (unsigned int)matrices[0].t[0] - (((unsigned int)random << 1) - 256);
            particle->y = matrices[0].t[1];
            random = Inv_ScrambleGrid();
            particle->z = (unsigned int)matrices[0].t[2] - (((unsigned int)random << 1) - 256);
            particle->velocityY = 0;
            particle->gravity = ((g_FieldFxCurrentAge >> 1) & 1) + 2;
        }
    }
        break;
    case 2:

    {
        int index = g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 16; g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16; g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.extent_x = 16; g_FieldFxSpriteParams.extent_y = 16;
        palette = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 2; g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0; g_FieldFxSpriteParams.depth = 0; g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}
