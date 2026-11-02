#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
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
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/room_m350_drop.h"
#include "pe1/room_m350_effects.h"

#include "m350_callbacks_externs.h"
#include "room_m350_data.h"
#include "room_m350_shared.h"

typedef struct {
    char reserved[24];
    unsigned char *state;
} Actor;

typedef struct {
    Actor *actor;
    char reserved04[10];
    unsigned char animation;
    unsigned char frameCount;
    int reserved10;
    union { int word; short halves[2]; } frame;
} Animation;

typedef struct { int reserved[19]; unsigned int flags; } ActorFlags;

typedef struct {
    ActorFlags *actor;
    char reserved04[504];
    int x, y, z;
} WorldActor;

typedef struct { unsigned int *actor; } RoomActor;

typedef RoomM350Vector Vector;

typedef GteMatrix Matrix;

typedef RoomM350Drop Particle;

typedef RenderMatrix Transform;

typedef FieldActor Instance;

typedef FieldActor Player;

typedef struct { int asset; short speed,targetSpeed,heightPhase,mode; } State;

typedef struct { short count,delay; } FadingModelState;

typedef struct { short count, delay; } EmissionState;

typedef struct { signed int scale:16; signed int shade:16; } EffectEntry;

int RoomM350_AreaController(int event) {
    Animation *animation = ANIM_OWNER_AS(Animation *)[2];
    if (event == 0) {
        g_RoomM350AreaRepeats = 4;
        g_RoomM350AreaDone = 0;
        g_RoomM350AreaPlayerInside = 0;
        g_RoomM350AreaEntered = 0;
    } else if (event == 1) {
        register short frame;
        if (animation->actor) {
            unsigned char *state = animation->actor->state;
            if (*state == 1) *state = 2;
        }
        frame = animation->frame.halves[1];
        if (animation->animation == 9) {
            short *remaining = &g_RoomM350AreaRepeats;
            if (*remaining > 0) {
                if (frame >= 22) {
                    animation->frame.word = 0x30000;
                    *remaining = *(volatile unsigned short *)remaining - 1;
                }
            } else if (frame >= animation->frameCount - 1) {
                if (animation->actor) *animation->actor->state = 4;
                g_RoomM350AreaDone = 1;
                return 1;
            }
        }
    }
    return 0;
}

/* Falling drop: moves until it passes the floor (splash) or touches the
 * player, otherwise queues its position for the trail; draws a glow and a
 * floor ring. */
int RoomM350_DropCallback(int mode, RoomM350Drop *drop) {
    GteShortVector position;

    if (mode == 1) {
        if (drop->landed) return 1;
        drop->x += drop->velocity.x;
        drop->y += drop->velocity.y;
        drop->z += drop->velocity.z;
        if (drop->y > g_RoomFloorY->y) {
            g_RoomEffectTrailPositions.splashed = 1;
            g_RoomEffectTrailPositions.splash_x = drop->x;
            g_RoomEffectTrailPositions.splash_z = drop->z;
            g_RoomEffectTrailPositions.touched = 1;
            g_RoomEffectTrailPositions.hit_x = drop->x;
            g_RoomEffectTrailPositions.hit_y = drop->y - 0xA0;
            g_RoomEffectTrailPositions.hit_z = drop->z;
            RoomM350_TestDropPlayerHit((short *)drop, 200);
            return 1;
        }
        if (RoomM350_TestDropPlayerHit((short *)drop, 0x50)) {
            g_RoomEffectTrailPositions.touched = 1;
            g_RoomEffectTrailPositions.hit_x = drop->x;
            g_RoomEffectTrailPositions.hit_y = drop->y;
            g_RoomEffectTrailPositions.hit_z = drop->z;
            drop->landed = 1;
        } else if (g_RoomEffectTrailPositions.count < 4) {
            GteShortVector *point =
                &g_RoomEffectTrailPositions.points[g_RoomEffectTrailPositions.count++];
            point->x = drop->x;
            point->y = drop->y;
            point->z = drop->z;
        }
    } else if (mode == 2) {
        int kind;
        int palette;
        position.x = drop->x;
        position.y = drop->y;
        position.z = drop->z;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        FieldFx_DrawShapeQuads(&position, 0, 0xA00, 0xA00,
                      (s16)g_FieldFxSpriteParams.parameter02 * 6 + 0x80,
                      GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                      1, 0x80, &g_RoomM350DropColors[0]);
        position.y = g_RoomFloorY->y;
        FieldFx_DrawStarFan(&position, 0x60, 0x60, 6, &g_RoomM350StarFanTilt, 0x1000, 0x1000,
                      &g_RoomM350DropColors[1], &g_RoomM350DropColors[2], 0x80, 1);
    }
    return 0;
}

int RoomM350_TestDropPlayerHit(short *point, int radius) {
    short *gate = &g_RoomM350DropHitCooldown;
    int dx, dz;
    WorldActor *world;
    if (*gate != 0) return 0;
    if (point[5] < g_RoomFloorY->y - 512) return 0;
    world = (WorldActor *)g_PlayerEntity;
    dx = world->x - point[4];
    if (dx < 0) dx = point[4] - world->x;
    if (dx > radius) return 0;
    dz = world->z - point[6];
    if (dz < 0) dz = point[6] - world->z;
    if (dz > radius) return 0;
    if (Math_IntSqrt(dx * dx + dz * dz) > radius) return 0;
    g_PlayerEntity->state->flags |= 0x4000;
    if (ANIM_OWNER_AS(RoomActor *)[2]->actor) *ANIM_OWNER_AS(RoomActor *)[2]->actor |= 0x80000000U;
    *gate = 30;
    return 1;
}

int RoomM350_DropEmitter(int event)
{
    Matrix matrix;
    Vector vector;
    if (event != 1) {
        if (event < 2) {
            if (event != 0) {
                return 0;
            }
        } else {
        goto done;
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 16, 4,
                             (FieldFxCallbackListCallback)RoomM350_DropCallback);
    }
update:
    {
        Particle *effect;
        Instance *instance;
        Transform *transform;
        Player *player;
        int dx, dz, dy;
        int angle;
        if (g_RoomM350DropDone) return 2;
        if (!g_RoomM350DropEmitterTrigger) return 0;
        effect = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!effect) return 0;
        instance = (Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
        transform = instance->render_object.matrices;
        player = g_PlayerEntity;
        effect->x = transform[7].translation[0];
        effect->y = transform[7].translation[1];
        effect->z = transform[7].translation[2];
        effect->landed = 0;
        dx = transform[7].translation[0] - player->render_object.model_matrix.translation[0];
        dz = transform[7].translation[2] - player->render_object.model_matrix.translation[2];
        dy = transform[7].translation[1] - player->render_object.model_matrix.translation[1];
        angle = ratan2(dy, Math_IntSqrt(dx * dx + dz * dz));
        angle += (signed char)Inv_ScrambleGrid() / 2;
        angle &= 0xFFF;
        vector.x = angle;
        if (angle < 0xD00) vector.x = 0xD00;
        {
            int offset = (signed char)Inv_ScrambleGrid();
            Vector *v = &vector;
            register Vector *input asm("$4") = v;
            register Matrix *m asm("$5") = &matrix;
            int yaw = (u16)instance->rot_y;
            /* Keep the rotation arguments ready before adding the yaw offset. */
            asm("" : : "r"(input), "r"(m));
            offset += 0x800;
            yaw += offset;
            vector.y = yaw;
            vector.z = 0;
            RotMatrixYXZ(input, m);
        }
        vector.x = 0;
        vector.y = 0;
        vector.z = 128;
        ApplyMatrixSV(&matrix, &vector, &effect->velocity);
        {
            RoomM350Actor *actor = ANIM_OWNER_AS(RoomM350Actor);
            g_RoomM350DropEmitterTrigger = 0;
            /* Publish consumption before loading the sound's position and owner. */
            asm volatile("" : : : "memory");
            {
                int x = effect->x;
                Asset_Find08w(0x60A, ((Instance *)actor->instance)->state->progress,
                    x, effect->y, effect->z);
            }
        }
        g_RoomM350DropLungeTimer = 8;
    }
done:
    return 0;
}

int RoomM350_DropHitCallback(int event, short *position) {
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        position[1] -= position[3];
        position[3] += 2;
    } else if (event == 2) {
        int kind = g_FieldFxSpriteParams.palette;
        int texture = g_FieldFxPaletteTable[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? texture + 6 : texture + 2);
        FieldFx_DrawShapeQuads((GteShortVector *)position, 0, 6144, 6144,
            (s16)g_FieldFxSpriteParams.parameter02 * (((g_FieldFxCurrentAge - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 6) & 0xFC0].bits.cos >> 5,
            &g_RoomM350DropHitColor);
    }
    return 0;
}

int RoomM350_DropHitEmitter(int event)
{
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 4,
                             (FieldFxCallbackListCallback)RoomM350_DropHitCallback);
        case 1:
    if (g_RoomM350DropDone) return 2;
    if (g_RoomM350DropHitTrigger) {
        short *effect = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!effect) return 0;
        {
            effect[0] = g_RoomM350DropHitPosition[0];
            effect[1] = g_RoomM350DropHitPosition[1];
            effect[2] = g_RoomM350DropHitPosition[2];
            effect[3] = 0;
         asm volatile(""); }
        g_RoomM350DropHitTrigger = 0;
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

/* Retail writes both fields twice during configuration. */

int RoomM350_DropTrailEmitter(int event)
{
    switch (event) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 16,
                             (FieldFxCallbackListCallback)RoomM350_DropTrailCallback);
    case 1:
    if (g_RoomM350DropDone) return 2;
    {
        int i;
        for (i = 0; i < g_RoomM350DropTrailCount; i++) {
            short *effect = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!effect) break;
            effect[0] = g_RoomEffectTrailPositions.points[i].x;
            effect[1] = g_RoomEffectTrailPositions.points[i].y;
            effect[2] = g_RoomEffectTrailPositions.points[i].z;
            /* Finish the copy before reloading the possibly aliased count. */
            asm("" : "=m"(g_RoomM350DropTrailCount) : "m"(g_RoomM350DropTrailCount), "m"(effect[2]));
        }
        g_RoomM350DropTrailCount = 0;
    }
    break;
    case 2:
    {
        int unit = 16;
        int index = g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter00 = unit;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;
        palette = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

int RoomM350_DropBurstCallback(int event, short *object)
{
    short position[4];
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int scale = (4096 - rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].part.sin) * 2;
        int *origin = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transforms[7].t;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = object[i] * scale / 4096 + origin[i];
        }
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)position, 0, 5120, 5120, (s16)g_FieldFxSpriteParams.parameter02 * 2 + 216,
            (unsigned short)handle, 1,
            (short)rcossin_tbl[((unsigned int)(g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 5,
            (struct RenderColor *)g_RoomM350DropBurstColor);
    }
    return 0;
}
/* Retail writes both fields twice during configuration. */

int RoomM350_DropBurstEmitter(int event)
{
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 16,
                             (FieldFxCallbackListCallback)RoomM350_DropBurstCallback);
        case 1:
    if (g_RoomM350DropDone) return 2;
    if (g_RoomM350DropBurstCount) {
        register int i asm("$18") = 0;
        while (i < 2) {
            Matrix matrix;
            Vector vector;
            int random;
            int highByte;
            int divisor;
            short *effect = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!effect) break;
            i++;
            vector.x = Inv_ScrambleGrid() << 4;
            vector.y = Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            /* Keep the two phases separate in CSE without emitting instructions. */
            asm volatile("" : : "i"(&&offset));
offset:
            vector.x = 0;
            vector.y = 0;
            highByte = Inv_ScrambleGrid();
            random = (highByte << 8) | Inv_ScrambleGrid();
            divisor = 64;
            vector.z = random % divisor + 256;
            ApplyMatrixSV(&matrix, &vector, (Vector *)effect);
        }
        {
            unsigned char *request = &g_RoomM350DropBurstCount;
            *request = *request - 1;
        }
    }
        break;
    case 2:

    {
        int unit = 16;
        int index = g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.parameter00 = unit;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;

        palette = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

int RoomM350_DropController(int event,short *cursor) {
    Events events=g_RoomM350DropEvents;
    Vector movement;
    Instance *instance=(Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    int frame,previous;
    short phase;
    if (event==0) {
        *(int *)&g_RoomM350DropEventSlots=0;
        g_RoomM350DropBurstState=0;
        g_RoomM350DropDone=0;
        g_RoomM350DropTrailCount=0;
        g_RoomM350DropLungeTimer=0;
        g_RoomM350DropHitCooldown=0;
        *cursor=0;
    } else {
        if (event!=1) return 0;
        frame=instance->anim.parts.integer;
        previous=(u16)(instance->anim_prev >> 16);
        if (instance->state && *instance->state->action_state==1)
        *instance->state->action_state=2;
        {
        short *timer=&g_RoomM350DropHitCooldown;
        if (*timer) --*timer;
        }
        if (instance->mode==6) {
        if ((short)frame<6) return 0;
        if ((short)previous<6) g_RoomM350DropBurstCount=5;
        return 0;
        }
        if (g_RoomM350DropRepeats>0) {
        if ((short)frame>=31) {
        instance->anim.fixed=0x60000;
        asm volatile("" ::: "memory");
        g_RoomM350DropRepeats--;
        }
        } else if ((short)frame>=instance->action-1) {
        if (instance->state) *instance->state->action_state=4;
        g_RoomM350DropDone=1;
        return 1;
        }
        if ((unsigned int)(frame-7)<8) phase=15-frame;
        else if ((unsigned int)(frame-19)<8) phase=27-frame;
        else phase=0;
        if (phase) {
        int speed=(short)rcossin_tbl[(((unsigned int)phase<<10)&0x3C00)/4].word>>6;
        int angle=FieldFx_VecToAngle(g_PlayerEntity->render_object.model_matrix.translation,instance->render_object.model_matrix.translation);
        instance->rot_y=FieldFx_TurnToward(instance->rot_y,(short)angle,speed);
        }
        while ((short)frame>=events.entries[*cursor].frame) {
        Event *entry=&events.entries[*cursor];
        if ((short)previous>=entry->frame) break;
        (&g_RoomM350DropEventSlots)[entry->slot]=entry->value;
        *cursor+=1;
        if ((unsigned short)*cursor>=8) *cursor=2;
        }
        {
        short *timer=&g_RoomM350DropLungeTimer;
        int remaining;
        remaining=*timer;
        if (remaining>0) {
        movement.y=0;
        movement.x=0;
        movement.z=g_RcossinTblCos[(((8-remaining)<<9)&0x3E00)/2]/256;
        ApplyMatrixSV(&instance->render_object.model_matrix,&movement,&movement);
        instance->pos_x+=(unsigned int)(int)movement.x<<16;
        instance->pos_z+=(unsigned int)(int)movement.z<<16;
        --*timer;
        }
        }
    }
done:
    return 0;
}

int RoomM350_RushFlareCallback(int event, RoomM350FlareParticle *particle)
{
    GteShortVector position;
    GteRotation rotation;
    if (event == 1) {
        int timer = g_FieldFxCurrentAge;
        if (timer >= 8) return 1;
        particle->shade = (short)rcossin_tbl[((unsigned int)timer << 8) & 0xF00].word >> 5;
    } else if (event == 2) {
        int kind, palette;
        unsigned short clut;
        position.x = 0;
        position.y = particle->y;
        position.z = particle->z;
        {
            register GteMatrix *matrix asm("$8") = &g_RoomM350RushMatrix;
            GteShortVector *out;
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            out = &position;
            gte_ldv0(out);
            gte_rtv0tr_mac();
            gte_stsv(out);
        }
        position.x += g_RoomM350RushCenter[0];
        position.y += g_RoomM350RushCenter[1];
        position.z += g_RoomM350RushCenter[2];
        rotation.x = (unsigned int)g_FieldFxCurrentAge * 384;
        rotation.y = ((FieldActor *)ANIM_OWNER_AS(RoomM350EffectActor)->instance)->rot_y;
        rotation.z = 0;
        rotation.flags = 1;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(0, palette);
        {
            int timer = g_FieldFxCurrentAge;
            register int scale asm("$4") = (s16)g_FieldFxSpriteParams.parameter02;
            register int phase asm("$3") = timer & 7;
            register int product asm("$8") = scale * phase;
            int texture = product + 64;
            asm("" : : "r"(product), "r"(texture));
            FieldFx_DrawShapeQuads(&position, &rotation, 0x2200, 4096,
                texture, clut, 1, particle->shade, (struct RenderColor *)&g_RoomM350RushFlareColors[timer & 3]);
        }
    }
    return 0;
}

/* Spawns flares and tests the quad swept by the actor's two side endpoints. */

/* The endpoint records' X words, read whole through GteShortVector views. */

int RoomM350_RushFlareEmitter(int event) {
    GteShortVector offset,vertices[4];
    GteShortVector *previousEndpoint;
    const GteShortVector *currentEndpoint;
    GteShortVector *previousOppositeEndpoint;
    const GteShortVector *currentOppositeEndpoint;
    int area;
    FieldActor *instance=(FieldActor *)ANIM_OWNER_AS(RoomM350EffectActor)->instance;
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,8,4,
                             (FieldFxCallbackListCallback)RoomM350_RushFlareCallback);
        case 1:
    previousEndpoint=&g_RoomM350RushPreviousEndpoint;
    previousOppositeEndpoint=&g_RoomM350RushPreviousOpposite;
    currentOppositeEndpoint=(const GteShortVector *)&g_RoomM350RushOppositeX;
    if(instance->anim.parts.integer>=31) return 2;
    currentEndpoint=(const GteShortVector *)&g_RoomM350RushEndpointX;
    if(!(g_FieldFxCurrentAge&1)) {
        RoomM350FlareParticle *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        if(particle) {
            int random=Inv_ScrambleGrid()&255;
            particle->y=random;
            particle->z=(random>>2)+64;
        }
    }
    if(g_RoomM350RushPlayerHit || instance->mode!=9) return 0;
    *previousEndpoint=*currentEndpoint;
    *previousOppositeEndpoint=*currentOppositeEndpoint;
    offset.x=256; offset.y=0; offset.z=0;
    ApplyMatrixSV((GteMatrix *)&instance->render_object.model_matrix,&offset,&offset);
    {
        int positionX=instance->render_object.model_matrix.translation[0];
        unsigned short x;
        unsigned short z;
        int positionZ;
        x=offset.x;
        z=offset.z;
        g_RoomM350RushEndpointX=x+positionX;
        positionZ=((volatile FieldActor *)instance)->render_object.model_matrix.translation[2];
        g_RoomM350RushEndpointValid=1;
        positionZ=z+positionZ;
        g_RoomM350RushEndpointZ=positionZ;
        g_RoomM350RushOppositeX=((volatile FieldActor *)instance)->render_object.model_matrix.translation[0]-x;
        g_RoomM350RushOppositeZ=((volatile FieldActor *)instance)->render_object.model_matrix.translation[2]-z;
    }
    asm volatile("" : : "m"(g_RoomM350RushOppositeZ));
    vertices[0]=*(GteShortVector *)&g_RoomM350RushEndpointX;
    vertices[1]=*(GteShortVector *)&g_RoomM350RushOppositeX;
    vertices[2]=g_RoomM350RushPreviousOpposite;
    {
        /* The saved endpoint uses its fourth halfword as a validity flag. */
        int valid=g_RoomM350RushPreviousEndpoint.pad;
        vertices[3]=g_RoomM350RushPreviousEndpoint;
        if(!valid) return 0;
        {
            int i=0;
            int previous=((unsigned int)vertices[3].z<<16)|(unsigned short)vertices[3].x;
            int point=((unsigned int)g_PlayerEntity->render_object.model_matrix.translation[2]<<16)|(unsigned short)g_PlayerEntity->render_object.model_matrix.translation[0];
            int *out=&area;
            for(;i<4;) {
                int current=((unsigned int)vertices[i].z<<16)|(unsigned short)vertices[i].x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(out);
                if(area<0) break;
                i++;
                previous=current;
            }
            if(i<4) return 0;
            {
                FieldActor *player=g_PlayerEntity;
                asm("" : : "r"(player));
                g_RoomM350RushPlayerHit=1;
                asm volatile("" : : "m"(g_RoomM350RushPlayerHit));
                player->state->flags|=0x4000;
                if(instance->state) instance->state->core_flags|=0x80000000u;
            }
        }
    }
        break;
    case 2:

    {
        short i=0;
        int index,color;
        short *destination=g_RoomM350RushCenter;
        GteMatrix *transforms;
        transforms=(GteMatrix *)instance->render_object.matrices;
        for(;i<3;i++) destination[i]=(transforms[11].t[i]+transforms[15].t[i])>>1;
        index=g_FieldFxTpageSlot3;
        g_FieldFxSpriteParams.parameter00=32; g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        color=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=3; g_FieldFxSpriteParams.parameter06=0;
        g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0;
        g_FieldFxSpriteParams.tpage=color;
    }
    }
    return 0;
}

int RoomM350_RushCloudCallback(int event,RoomM350CloudParticle *particle)
{
    if(event==1) {
        int timer=g_FieldFxCurrentAge;
        int shade;
        int size;
        int product;
        int tripledProduct;
        if(timer>=8) return 1;
        shade=rcossin_tbl[((unsigned int)timer*3<<6)&0xFC0].bits.cos >> 5;
        particle->state.value.active.secondShade=128-((unsigned int)timer<<4);
        size=particle->size;
        particle->state.value.active.firstShade=shade;
        product=rcossin_tbl[((unsigned int)timer<<7)&0xF80].part.sin*size;
        {
            int twice=(unsigned int)product<<1;
            tripledProduct=(unsigned int)twice+(unsigned int)product;
        }
        particle->state.value.active.animatedSize=tripledProduct/4096;
    } else if(event==2) {
        GteShortVector world;
        /* Match note: original purpose of unused sp+0x30..0x37 is unknown. */
        char frameGap[8];
        world.x=(unsigned int)particle->anchor[0]+(unsigned short)particle->position[0];
        world.y=(unsigned int)particle->anchor[1]+(unsigned short)particle->position[1];
        world.z=(unsigned int)particle->anchor[2]+(unsigned short)particle->position[2];
        if(particle->state.value.active.firstShade>0) {
            int kind=g_FieldFxSpriteParams.palette;
            int palette=g_FieldFxPaletteTable[kind];
            unsigned short clut;
            if(kind==4 && g_FieldFxAltPaletteFlag) palette+=4;
            clut=GetClut(32,palette);
            FieldFx_DrawShapeQuads(&world,0,particle->size,particle->size,(s16)g_FieldFxSpriteParams.parameter02*2+216,clut,1,particle->state.value.active.firstShade,&g_RoomM350GlowSpriteColor);
        }
        {
            int kind=g_FieldFxSpriteParams.palette;
            int palette=g_FieldFxPaletteTable[kind];
            unsigned short clut;
            if(kind==4 && g_FieldFxAltPaletteFlag) palette+=4;
            clut=GetClut(32,palette);
            FieldFx_DrawShapeQuads(&world,0,particle->state.value.active.animatedSize,particle->state.value.active.animatedSize,(s16)g_FieldFxSpriteParams.parameter02*2+216,clut,1,particle->state.value.active.secondShade,&g_RoomM350GlowSpriteColor);
        }
    }
    return 0;
}
int RoomM350_RushCloudEmitter(int event)
{
    GteMatrix matrix;
    GteShortVector vector;
    switch (event) {
    case 0:

        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,20,16,
                             (FieldFxCallbackListCallback)RoomM350_RushCloudCallback);
        case 1:
    {
        int i;
        if(g_RoomM350RushState.complete) return 2;
        for(i=0;i<2;i++) {
            int firstRandom;
            int combinedRandom;
            int divisor;
            RoomM350CloudParticle *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
            if(!particle) return 0;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.z=0;
            RotMatrixYXZ(&vector,&matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x=0; vector.y=0;
            firstRandom=Inv_ScrambleGrid();
            combinedRandom=((unsigned int)firstRandom<<8)|(unsigned int)Inv_ScrambleGrid();
            divisor=128;
            vector.z=combinedRandom%divisor;
            ApplyMatrixSV(&matrix,&vector,(GteShortVector *)particle->position);
            particle->position[1]=(unsigned short)particle->position[1]*2+128;
            particle->size=((unsigned int)Inv_ScrambleGrid()<<4)+4096;
            particle->anchor=ANIM_OWNER_AS(RoomM350EffectActor)->instance->transforms[i ? 11:15].t;
        }
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.parameter00 = 16;
    g_FieldFxSpriteParams.parameter02 = 1;
    g_FieldFxSpriteParams.extent_x = 16;
    g_FieldFxSpriteParams.extent_y = 16;
    g_FieldFxSpriteParams.extent_x = 16;
    g_FieldFxSpriteParams.extent_y = 16;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
    g_FieldFxSpriteParams.palette = 2;
    g_FieldFxSpriteParams.parameter06 = 0;
    g_FieldFxSpriteParams.depth = 16;
    }
    return 0;
}

int RoomM350_RushController(int event,State *state)
{
    Instance *instance=(Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    Vector offset;
    /* Match note: original purpose of unused sp+0x20..0x2F is unknown. */
    char frameGap[16];
    if(event==0) {
        RenderMatrix *matrix;
        state->speed=0; state->targetSpeed=0; state->heightPhase=0; state->mode=0;
        g_RoomM350RushState.complete=0; g_RoomM350RushPlayerHit=0; g_RoomM350RushEndpointValid=0;
        asm volatile("" : : : "memory");
        matrix=instance->render_object.matrices;
        state->asset=Asset_Find08w(0x5CB,instance->state->progress,
            (short)matrix->translation[0],(short)matrix->translation[1],(short)matrix->translation[2]);
    } else if(event==1) {
        int frame,angle,delta,cosine,sine;
        int phase;
        int target;
        register int height asm("$2");
        register int speed asm("$5");
        if(instance->mode!=9) return 0;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        {
            register int current asm("$3")=instance->anim.parts.integer;
            frame=current;
        if(g_RoomM350RushState.repeats>0) {
            height=(short)current;
            if(height>=30) {
                instance->anim.fixed=0x50000;
                g_RoomM350RushState.repeats--;
            }
        } else if((short)current>=instance->action-1) {
            if(instance->state) instance->state->action_state[0]=4;
            g_RoomM350RushState.complete=1;
            Akao_Cmd_21(state->asset,0);
            return 1;
        }
        }
        g_RoomM350RushState.playerDistance=FieldFx_GetDistanceXZ((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
        angle=FieldFx_VecToAngle((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x)&4095;
        g_RoomM350RushState.playerAngle=angle;
        delta=(angle-(unsigned short)instance->rot_y)&4095;
        {
            if (state->mode != 0) {
                if (state->mode != 1) {
                    state->targetSpeed=0;
                    goto apply_cruise_speed;
                }
            } else {
                if(state->speed<=1280) instance->rot_y=FieldFx_TurnToward(instance->rot_y,angle,80);
                if(state->speed<=state->targetSpeed && ((instance->rot_y-g_RoomM350RushState.playerAngle+64)&4095)<=256) {
                    state->mode=1; state->targetSpeed=1536; g_RoomM350RushPlayerHit=0;
                }
                goto apply_cruise_speed;
            }
            if(state->targetSpeed>=state->speed && ((delta-1536)&4095)<1024) {
                state->mode=0; state->targetSpeed=512;
            }
        }
apply_cruise_speed:
        speed=state->speed; target=state->targetSpeed;
        {
            register int next asm("$3")=speed;
            int result;
            if(speed<target) {
                result=next+128;
                state->speed=result;
            } else {
                result=target<speed;
                if(result) {
                    result=next-32;
                    state->speed=result;
                }
            }
        }
        cosine=rcos(instance->rot_y+2048);
        sine=rsin(instance->rot_y+2048);
        {
        GteMatrix *matrix=&g_RoomM350RushMatrix;
        Vector *vector=&offset;
        matrix->m[0][0]=cosine; g_RoomM350RushMatrix.m[0][2]=sine; g_RoomM350RushMatrix.m[2][0]=0u-(unsigned int)sine;
        g_RoomM350RushMatrix.m[2][2]=cosine;
        g_RoomM350RushMatrix.t[2]=0; g_RoomM350RushMatrix.t[1]=0; g_RoomM350RushMatrix.t[0]=0;
        g_RoomM350RushMatrix.m[2][1]=0; g_RoomM350RushMatrix.m[1][2]=0; g_RoomM350RushMatrix.m[1][0]=0; g_RoomM350RushMatrix.m[0][1]=0;
        g_RoomM350RushMatrix.m[1][1]=4096;
        offset.x=0; offset.y=0; offset.z=state->speed;
        ApplyMatrixSV(matrix,vector,vector);
        }
        instance->pos_x=(unsigned int)instance->pos_x+((unsigned int)offset.x<<12);
        instance->pos_z=(unsigned int)instance->pos_z+((unsigned int)offset.z<<12);
        phase=state->heightPhase;
        {
        int nextPhase=phase;
        asm("" : "=r"(nextPhase) : "0"(nextPhase));
        if(phase<16) {
            int sample,floor;
            phase=nextPhase+1;
            state->heightPhase=phase;
            sample=rcossin_tbl[((unsigned int)phase<<6)&0xFC0].part.sin;
            height=4096-sample;
            floor=g_RoomFloorY->y;
            height=height/8-128;
            height=floor-height;
            instance->pos_y=(unsigned int)height<<16;
        } else if((short)frame>=31) {
            int floor;
            state->mode=2;
            floor=(unsigned int)g_RoomFloorY->y<<16;
            if(floor<instance->pos_y) instance->pos_y=(unsigned int)instance->pos_y-0x100000;
            else instance->pos_y=floor;
        }
        }
    }
    return 0;
}

int RoomM350_BodyDebrisCallback(int event, RoomM350FadingParticle *object)
{
    /* Retail initializes two unused stack words; their original type is unknown. */
    struct { int x, y; } unused;

    unused.x = 0;
    unused.y = 0;
    if (event == 1) {
        short phase = object->counter >> 1;
        object->phase = phase;
        if (phase >= 8) {
            return 1;
        }
        if (phase >= 4) {
            object->brightness -= 8;
        }
        {
            unsigned int counter = (unsigned short)object->counter;
            register short velocity;
            velocity = object->velocity >> 1;
            object->counter = counter + 1;
            object->position.y -= velocity;
            object->velocity += object->acceleration;
        }
    } else if (event == 2) {
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6 : palette + 2);
        FieldFx_DrawShapeQuads(&object->position, 0, object->size, object->size, (s16)g_FieldFxSpriteParams.parameter02 * object->phase,
            (unsigned short)handle, 1, object->brightness, 0);
    }
    return 0;
}
int RoomM350_BodyDebrisEmitter(int event,FadingModelState *state)
{
    Vector offset;
    RenderMatrix rotation;
    /* Match note: original purpose of unused sp+0x40..0x47 is unknown. */
    char frameGap[8];
    switch (event) {
    case 0:

        {
            Instance *instance=(Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
            RenderMatrix *matrix=instance->render_object.matrices;
            Asset_Find08w(0x549,instance->state->progress,(short)matrix->translation[0],(short)matrix->translation[1],(short)matrix->translation[2]);
            state->count=0; state->delay=0; g_RoomM350BodyDone=0;
            return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350Emitter)->pool,20,10,
                                 (FieldFxCallbackListCallback)RoomM350_BodyDebrisCallback);
        }
        case 1:
    {
        RoomM350FadingParticle *particle;
        RoomM350Actor *actor;
        Instance *instance;
        int random;
        int scatterRandom;
        int index;
        int halfSize;
        int sizeBits;
        int cosine,divisor;
        int sine;
        int product;
        if(g_RoomM350BodyDone) return 2;
        state->delay=(unsigned short)state->delay+1;
        if(state->delay<4) return 0;
        particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if(!particle) return 0;
        rand();
        particle->brightness=64;
        {
            int count=state->count;
                        particle->size=((unsigned int)count<<7)+4096;
        }
        random=Inv_ScrambleGrid();
        actor=ANIM_OWNER_AS(RoomM350Actor);
        instance=(Instance *)actor->instance;
        {
            register int low asm("$5")=random&255;
            register int parts asm("$3")=instance->render_object.header->part_count-1;
            register int scaled asm("$2")=low*parts;
                        scaled=scaled/256;
                        {
                parts=scaled+1;
                asm volatile("" : : "r"(parts));
                index=parts;
            }
        {
        Instance *ySource;
        Instance *zSource;
        particle->position.x=instance->render_object.matrices[index].translation[0];
                ySource=(Instance *)actor->instance;
        asm volatile("" : : "r"(ySource));
        parts=particle->size;
        asm volatile("" ::: "memory");
        sizeBits=(unsigned short)particle->size;
        particle->position.y=ySource->render_object.matrices[index].translation[1];
        asm volatile("" : : : "$2");
        {
            u8 low7=random&127;
            product=low7*parts;
        }
        zSource=(Instance *)actor->instance;
        asm volatile("" : : "r"(zSource));
        sizeBits=(unsigned int)sizeBits<<16;
                {
        RenderMatrix *transforms=zSource->render_object.matrices;
        asm volatile("" : : "r"(transforms));
        halfSize=sizeBits>>17;
        asm volatile("" : : "r"(transforms));
        particle->position.z=transforms[index].translation[2];
        }
        }
        particle->velocity=0; particle->acceleration=0; particle->counter=0;
        {
            int y=(unsigned short)particle->position.y;
            asm volatile("" : "=l"(product) : "0"(product), "r"(y), "r"(halfSize));
            particle->position.y=y-(product>>13);
        }
        offset.x=0;
        scatterRandom=rand(); divisor=96;
        offset.z=((scatterRandom%divisor+64)*halfSize)>>12;
        cosine=rcos(rand());
        sine=rsin(rand());
        rotation.rotation[0][2]=sine;
        sine=0u-(unsigned int)sine;
        rotation.rotation[2][0]=sine;
        sine=4096;
        rotation.rotation[1][1]=sine;
                rotation.rotation[0][0]=cosine; rotation.rotation[2][2]=cosine;
        rotation.translation[2]=0; rotation.translation[1]=0; rotation.translation[0]=0;
        rotation.rotation[2][1]=0; rotation.rotation[1][2]=0;
        rotation.rotation[1][0]=0; rotation.rotation[0][1]=0;
        {
            RenderMatrix *matrix=&rotation;
            Vector *out;
            gte_ldrotmatrix(matrix); gte_ldtransmatrix(matrix);
            out=&offset;
            gte_ldv0(out); gte_rtv0tr_mac();
            gte_stsv(out);
        }
        particle->position.x=(unsigned short)particle->position.x+(unsigned short)offset.x;
        particle->position.z=(unsigned short)particle->position.z+(unsigned short)offset.z;
        state->delay=0; state->count=(unsigned short)state->count+1;
        Asset_Find08w(0x5FE,((Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance)->state->progress,particle->position.x,particle->position.y,particle->position.z);
        }
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 32;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 32;
    g_FieldFxSpriteParams.extent_y = 32;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 1;
    g_FieldFxSpriteParams.parameter0A = 0;
    g_FieldFxSpriteParams.depth = 32;
    }
    return 0;
}

int RoomM350_BodyRingCallback(int event, RoomM350SineParticle *object)
{
    GteShortVector position;
    GteRotation texture;
    if (event == 1) {
        int wave = rsin((unsigned int)g_FieldFxCurrentAge << 4);
        object->size = wave * object->amplitude >> 12;
        if (g_FieldFxCurrentAge >= 8) {
            object->brightness -= 8;
            if (object->brightness < 16) return 1;
        }
    } else if (event == 2) {
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        unsigned short handle;
        handle = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7 : palette + 3);
        FieldFx_DrawShapeQuads(&object->position, &object->rotation, object->size, object->size,
            128, handle, 1, object->brightness, 0);
        position.x = object->position.x;
        position.y = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transform.t[1];
        position.z = object->position.z;
        texture.x = 1024;
        texture.y = g_FieldFxCurrentAge * 192;
        texture.z = 0;
        texture.flags = 1;
        FieldFx_DrawStarFan(&position, 384, 384, 10, &texture, object->size,
            object->size, &g_RoomM350BodyRingColor0, &g_RoomM350BodyRingColor1, object->brightness, 1);
    }
    return 0;
}

int RoomM350_BodyRingEmitter(int event, EmissionState *state)
{
    switch (event) {
    case 0:

        state->count = 0;
        state->delay = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM350EffectEmitter)->pool, 24, 8,
                             (FieldFxCallbackListCallback)RoomM350_BodyRingCallback);
        case 1:
    if (state->count < 24) {
        RoomM350SineParticle *effect;
        int random;
        int index;
        RoomM350EffectActor *actor;
        FieldActor *instance;
        state->delay--;
        if (state->delay != -1) return 0;
        effect = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        if (!effect) return 0;
        random = Inv_ScrambleGrid();
        actor = ANIM_OWNER_AS(RoomM350EffectActor);
        instance = (FieldActor *)actor->instance;
        index = (unsigned char)random;
        {
            register int byte asm("$4") = index;
            int count = instance->render_object.header->part_count;
            register int range asm("$3") = count - 1;
            asm("" : "=r"(range) : "0"(range), "r"(count));
            index = byte * range / 256 + 1;
        }
        effect->position.x = instance->render_object.matrices[index].translation[0];
        effect->position.y = ((FieldActor *)actor->instance)->render_object.matrices[index].translation[1];
        effect->position.z = ((FieldActor *)actor->instance)->render_object.matrices[index].translation[2];
        {
            int count = state->count;
            effect->brightness = 128;
            effect->amplitude = count * 2048 + 2048;
        }
        /* Keep the position/amplitude stores before the delay/count update. */
        asm volatile("" : : "m"(effect->amplitude));
        {
            int delay = random & 7;
            unsigned short count = state->count;
            delay += 12;
            state->delay = delay;
            state->count = count + 1;
        }
        {
            register int random asm("$4") = rand();
            int high;
            int y;
            int rawHigh;
            int bit;
            effect->rotation.x = (random & 255) + 512;
            asm volatile("" : : "m"(effect->rotation.x));
            rawHigh = random >> 8;
            high = rawHigh & 255;
            asm("" : : "r"(high));
            bit = random & 2;
            asm("" : : "r"(bit), "r"(random));
            if (bit) y = high + 768;
            else y = high - 1024;
            effect->rotation.y = y;
        }
        effect->rotation.z = 0;
        effect->rotation.flags = 0;
    } else {
        g_RoomM350BodyDone = 1;
        return 2;
    }
        break;
    case 2:

    g_FieldFxSpriteParams.parameter00 = 64;
    g_FieldFxSpriteParams.parameter02 = 4;
    g_FieldFxSpriteParams.extent_x = 64;
    g_FieldFxSpriteParams.extent_y = 64;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 1;
    g_FieldFxSpriteParams.parameter0A = 5;
    g_FieldFxSpriteParams.depth = 0;
    }
    return 0;
}

/* Walks the actor list for the first live actor of the given type and sub
 * id and leaves it in g_RoomM350HandActor. */
FieldActor **RoomM350_HandConfigure(s32 arg0, s32 arg1, s32 arg2) {
    FieldActor **slot;
    FieldActor **iter;
    FieldActor *node;

    slot = &g_RoomM350HandActor;
    if ((*slot = g_FieldActorListHead) != 0) {
        iter = slot;
        while (1) {
            node = g_RoomM350HandActor;
            if ((node->type_id == arg1) && (node->sub_id == arg2) && ((node->flags & 0x10) == 0)) {
                break;
            }

            if ((*iter = g_RoomM350HandActor->next) == 0) {
                break;
            }
        }
    }

    return &g_RoomM350HandActor;
}

s32 RoomM350_HandSparkCallback(s32 mode, GteShortVector *ctx) {
    s32 tex;

    if (mode == 1) {
        if (g_FieldFxCurrentAge < 8) {
            goto ret0;
        }
        return 1;
    }

    if (mode != 2) {
        return 0;
    }

    tex = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
    if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
        tex += 4;
    }

    FieldFx_DrawShapeQuads(ctx, 0, 0x1800, 0x1800, (s16)g_FieldFxSpriteParams.parameter02 + 0xDC, GetClut(0x30, tex), 1,
                  (rcossin_tbl[((g_FieldFxCurrentAge - 1) << 8) & 0xF00].word << 16) >> 21, &g_RoomM350HandSparkColor);
ret0:
    return 0;

}

/* Walks the actor list for the first live actor of the given type and sub
 * id, leaves it in g_RoomM350OrbActor and returns the joint position slot. */
GteShortVector *RoomM350_OrbConfigure(s32 arg0, s32 arg1, s32 arg2) {
    FieldActor **slot;
    FieldActor **iter;
    FieldActor *node;

    slot = &g_RoomM350OrbActor;
    if ((*slot = g_FieldActorListHead) != 0) {
        iter = slot;
        while (1) {
            node = g_RoomM350OrbActor;
            if ((node->type_id == arg1) && (node->sub_id == arg2) && ((node->flags & 0x10) == 0)) {
                break;
            }

            if ((*iter = g_RoomM350OrbActor->next) == 0) {
                break;
            }
        }
    }

    return &g_RoomM350OrbSpawnPoint;
}

char *RoomM350_AreaConfigure(void *arg0, char arg1, short arg2, short arg3) {
    char *ptr = &g_RoomM350AreaTrackPlayer;

    *ptr = arg1;
    g_RoomM350AreaConfigYaw[0] = arg2;
    g_RoomM350AreaConfigYaw[1] = arg3;
    return ptr - 0x29;
}

s32 RoomM350_AreaPlayerPulseEmitter(s32 arg0) {
    char *obj;
    char *actor;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        obj = ANIM_POOL_AS(char *)[2];
        return FieldFx_InitCallbackList(obj, 8, 2,
                             (FieldFxCallbackListCallback)RoomM350_AreaPlayerPulseCallback);
    } else if ((char)g_RoomM350AreaDone != 0) {
        return 2;
    } else if ((char)g_RoomM350AreaEntered != 0) {
        obj = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(char *)[2]);
        if (obj != 0) {
            actor = (char *)D_8009CD10;
            *(short *)&obj[0] = *(s32 *)&actor[0x1FC];
            *(short *)&obj[2] = *(s32 *)&actor[0x200] - 0x140;
            {
                s32 value = *(s32 *)&actor[0x204];
                *(short *)&obj[4] = value;
            }
            g_RoomM350AreaEntered = 0;
        }
    }

    return 0;
}

short *RoomM350_DropConfigure(void *arg0, short arg1) {
    short *ptr = &g_RoomM350DropRepeats;

    *ptr = arg1;
    return ptr - 0x15;
}

s32 RoomM350_DropFlareEmitter(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 4, 2,
                             (FieldFxCallbackListCallback)RoomM350_DropFlareCallback);
    } else if ((char)g_RoomM350DropDone != 0) {
        return 2;
    } else if (g_RoomM350DropEventSlots != 0) {
        obj = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
        if (obj != 0) {
            /* The translation of the owner's eighth matrix. */
            *(s32 **)obj = g_FieldFxOwner->actor->render_object.matrices[7].translation;
            g_RoomM350DropEventSlots = 0;
        }
    }

    return 0;
}

s32 RoomM350_DropSplashEmitter(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(char *)[2], 8, 6,
                             (FieldFxCallbackListCallback)RoomM350_DropSplashCallback);
    } else if ((char)g_RoomM350DropDone != 0) {
        return 2;
    } else if (g_RoomM350DropSplashTrigger != 0) {
        obj = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(char *)[2]);
        if (obj != 0) {
            *(u16 *)&obj[0] = g_RoomM350DropSplashPosX;
            *(u16 *)&obj[2] = g_RoomFloorY->raw;
            *(u16 *)&obj[4] = g_RoomM350DropSplashPosZ;
            g_RoomM350DropSplashTrigger = 0;
        }
    }

    return 0;
}

s32 RoomM350_DropTrailCallback(s32 mode, void *ctx) {
    s32 tex;

    if (mode == 1) {
        if (g_FieldFxCurrentAge < 5) {
            goto ret0;
        }
        return 1;
    }

    if (mode != 2) {
        return 0;
    }

    if (g_FieldFxCurrentAge < 2) {
        return 0;
    }

    tex = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
    if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
        tex += 4;
    }

    FieldFx_DrawShapeQuads(ctx, 0, 0x1000, 0x1000, 0xDC, GetClut(0x30, tex), 1, (6 - g_FieldFxCurrentAge) << 5, &g_RoomM350DropTrailColor);
ret0:
    return 0;
}

short *RoomM350_RushConfigure(void *arg0, short arg1) {
    short *ptr = &g_RoomM350RushState.repeats;

    *ptr = arg1;
    return ptr - 0x24;
}

int RoomM350_DropFlareCallback(int event, int **position) {
    short point[4];
    int shade;
    int *colors;
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 4) return 1;
    } else if (event == 2) {
        point[0] = (*position)[0];
        point[1] = (*position)[1];
        point[2] = (*position)[2];
        colors = g_RoomM350DropFlareColors;
        shade = g_RcossinTblCos[((g_FieldFxCurrentAge - 1) << 9) & 0x1E00];
        FieldFx_DrawStarFan((struct GteShortVector *)point, 512, 512, 16, 0, (short)shade, (short)shade,
                     (struct RenderColor *)colors, (struct RenderColor *)(colors + 1), (short)shade >> 5, 1);
    }
    return 0;
}

int RoomM350_DropSplashCallback(int event, void *context) {
    short rotation[4];
    short *entry;
    int scale;
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (event == 2) {
        int angle = (g_FieldFxCurrentAge << 12) / 20;
        unsigned int offset = (unsigned int)(g_FieldFxCurrentAge - 1) << 8;
        short *base = ((short *)rcossin_tbl);
        entry = (short *)((offset & 0x3F00) + (char *)base);
        scale = entry[0];
        rotation[0] = 1024;
        rotation[1] = 0;
        rotation[2] = angle;
        rotation[3] = 1;
        FieldFx_DrawStarFan(context, 384, 384, 10, (struct GteRotation *)rotation, scale, scale,
                     &g_RoomM350BlackColor, (struct RenderColor *)g_RoomM350DropSplashColor,
                     ((EffectEntry *)entry)->shade >> 5, 1);
    }
    return 0;
}
