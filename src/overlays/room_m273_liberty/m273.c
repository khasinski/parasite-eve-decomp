#include "common.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_vector_math.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/floor_polygon.h"
#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/gte_sine_table.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_libc.h"
#include "pe1/room_floor.h"
#include "pe1/room_liberty_choice.h"
#include "pe1/scene_archive.h"
#include "pe1/scene_assets.h"
#include "pe1/scene_grab_head.h"

#include "m273_externs.h"
#include "m273_externs_1.h"
#include "m273_externs_2.h"
#include "room_m273.h"
#include "room_m273_boss.h"
#include "room_m273_boss_state.h"
#include "room_m273_effects.h"
#include "room_m273_particles.h"
#include "room_m273_sway.h"
#include "room_m273_sweep.h"

typedef struct { Vector position; unsigned char parameter[4]; } RoomM273ScaledLayerEffect;

typedef RoomM273ThresholdTransform Transform;

typedef RoomM273ScaledLayerOwner Owner;

typedef struct { short x, y, z, pad; } Sprite;

typedef GteMatrix ThresholdTripleMatrix;

typedef GteVector ThresholdTripleVector;

/* Ring band record the pair callback draws: anchor, scale and intensity. */
typedef struct { GteShortVector position; short scale; short pad0A; short intensity; } ThresholdPairBand;

typedef struct RoomM273RingController {
    u8 reserved_00[8];
    FieldActor *actor;
} RoomM273RingController;

typedef RoomM273RisingParticle Particle;

typedef GteMatrix Matrix;

typedef FieldActor Player;

typedef RoomM273PairPoolRecord Record;

typedef GteShortVector Position;

void RoomLib_GrabOnApproach(SceneGrabHead *head) {
    RoomSelectionState *typedState = (RoomSelectionState *)head;
    char *stateBytes = (char *)typedState;
    FieldActor *actor = typedState->actor;
    FieldActor *player = g_PlayerEntity;

    if (player->mode < 18) {
        int frame = actor->anim.parts.integer;

        if (frame < 14) {
            return;
        }
        if (frame < 30) {
            void *origin = actor->render_object.model_matrix.translation;
            int mapAngle;

            *(int *)0x1F800008 =
                *(int *)&(*(char **)&actor->render_object.matrices)[0x594] << 16;
            *(int *)0x1F800010 =
                *(int *)&(*(char **)&actor->render_object.matrices)[0x59C] << 16;
            mapAngle = FieldFx_VecToAngle(
                origin, (Vec3Fixed *)(*(char **)&actor->render_object.matrices + 0x594));

            if (((FieldFx_VecToAngle(
                      origin, (Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation) -
                  mapAngle + 0x40) &
                 0xFFF) < 0x100) {
                if (FieldFx_GetDistanceXZ((Vec3Fixed *)&g_PlayerEntity->pos_x, (void *)0x1F800008) <
                    0x180) {
                    typedState->callback = RoomM273_GrabHoldStep;
                    Battle_BeginPlayerAction();
                    {
                        FieldActor *activePlayer = g_PlayerEntity;

                        activePlayer->render_object.animation_source = &actor->render_object;
                        activePlayer->render_object.animation_state = 4;
                        activePlayer->render_object.animation_id = 44;
                        activePlayer->flags |= 0x10000;
                        activePlayer->render_object.flags_9C |= 0x400;
                        activePlayer->render_object.model_matrix = typedState->matrix;
                    }
                    typedState->activated = 1;
                    {
                        FieldActor *positionPlayer = g_PlayerEntity;

                        typedState->savedX = positionPlayer->pos_x;
                        typedState->savedZ = positionPlayer->pos_z;
                    }
                }
            }
            return;
        }
    }

    *((RoomSelectionState *)stateBytes)->signal = 3;
    ((RoomSelectionState *)stateBytes)->callback =
        RoomM273_GrabWaitRelease;
}

void RoomM273_GrabHoldStep(RoomSelectionState *state) {
    FieldActor *actor = state->actor;
    RoomPlacementState *stateTail = (RoomPlacementState *)&state->callback;
    u16 currentFrame;
    u16 previousFrame;

    g_PlayerEntity->render_object.model_matrix = state->matrix;
    currentFrame = (unsigned int)actor->anim.fixed >> 16;
    previousFrame = (unsigned int)actor->anim_prev >> 16;
    g_PlayerEntity->pos_x = state->savedX;
    g_PlayerEntity->pos_z = state->savedZ;

    if (!stateTail->placement_checked) {
        FieldActorState *playerState = g_PlayerEntity->state;
        if (playerState == 0 || playerState->amount <= 0) {
            RoomM273_ReturnPlayerToPlacement(actor, stateTail);
            stateTail->phase_countdown = 0;
            stateTail->placement_checked = 1;
        }
    }

    switch (actor->mode) {
    case 16:
        if ((s16)currentFrame >= 33 && (s16)previousFrame < 33) {
            actor->anim.fixed = 0x3D0000;
        } else if ((s16)currentFrame >= actor->action - 1) {
            *stateTail->signal = 2;
        }
        return;

    case 9:
        if ((s16)previousFrame < 24 && (s16)currentFrame >= 24) {
            if (stateTail->phase_countdown > 0) {
                actor->anim.fixed = 0;
                --stateTail->phase_countdown;
            }
        } else if ((s16)previousFrame <= 0 && (s16)currentFrame > 0) {
            u8 *substate = ((RoomM273ActorState *)actor->state)->substate;
            substate[1] = stateTail->pending_action;
        } else if ((s16)previousFrame < 34 && (s16)currentFrame >= 34) {
            s32 delta = ((int)(s16)g_RoomFloorY->raw << 16) - actor->pos_y;
            if (delta < 0) {
                delta += 15;
            }
            stateTail->vertical_step = 4;
            stateTail->vertical_step = delta >> stateTail->vertical_step;
            stateTail->vertical_ticks = 16;
        } else if ((s16)previousFrame < 50 && (s16)currentFrame >= 50) {
            if (stateTail->placement_checked == 0) {
                Battle_ReturnToIdle();
                RoomM273_ReturnPlayerToPlacement(actor, stateTail);
            }
        } else if ((s16)previousFrame < 3 && (s16)currentFrame >= 3) {
            FieldActorState *playerState = g_PlayerEntity->state;
            if (playerState != 0 && playerState->amount > 0) {
                playerState->flags |= 0x4000;
                if (actor->state != 0) {
                    actor->state->core_flags |= 0x80000000;
                }
            }
        } else if ((s16)currentFrame >= actor->action - 1) {
            if (actor->state != 0) {
                u8 *substate = ((RoomM273ActorState *)actor->state)->substate;
                substate[0] = 4;
            }
            RoomM273_GrabRelease(state);
        }
        if (stateTail->vertical_ticks != 0) {
            actor->pos_y += stateTail->vertical_step;
            --stateTail->vertical_ticks;
        }
        return;

    case 8:
        if ((s16)currentFrame <= 0) {
            FieldActorState *actorState = actor->state;
            if (actorState != 0) {
                u8 *substate = ((RoomM273ActorState *)actorState)->substate;
                substate[0] = 2;
            }
        }
        return;

    default:
        RoomM273_GrabRelease(state);
        return;
    }
}

void RoomM273_ReturnPlayerToPlacement(RoomPlacementOwner *owner, RoomPlacementState *state) {
    FieldActor *player = g_PlayerEntity;
    RoomPlacementMap *placement;
    int *script = g_FloorPolygonVertices;

    player->render_object.animation_source = 0;
    player->render_object.animation_state = 0;
    player->flags &= ~0x10000;
    player->render_object.flags_9C &= ~0x400;

    placement = owner->map;
    if (Geo_PointInPoly(placement->x << 16, placement->z << 16,
                      script, g_FloorPolygonVertexCount)) {
        g_PlayerEntity->pos_x = owner->map->x << 16;
        g_PlayerEntity->pos_z = owner->map->z << 16;
    } else {
        g_PlayerEntity->pos_x = state->fallback_x;
        g_PlayerEntity->pos_z = state->fallback_z;
    }

    Entity_FindFloor(g_PlayerEntity);

    {
        FieldActor *updatedPlayer = g_PlayerEntity;
        GeomScrollState *scrollState;

        updatedPlayer->base_x = updatedPlayer->pos_x;
        scrollState = &g_GeomScroll.state;
        updatedPlayer->base_y = updatedPlayer->pos_y;
        updatedPlayer->base_z = updatedPlayer->pos_z;
        updatedPlayer->motion_x = 0;
        updatedPlayer->motion_y = 0;
        updatedPlayer->motion_z = 0;
        updatedPlayer->accel_x = 0;
        updatedPlayer->accel_y = 0;
        updatedPlayer->accel_z = 0;
        scrollState->flags |= 0x80;
        state->active = 0;
    }
}

void RoomM273_GrabWaitRelease(RoomSelectionState *state) {
    FieldActor *slot = state->actor;

    if (slot->anim.parts.integer >= 0x3B) {
        RoomM273_GrabRelease(state);
    }
}

s32 RoomM273_GrabRelease(RoomSelectionState *selection) {
    RoomPlacementState *placementState;
    char *target;
    FieldActorState *actorState;

    actorState = selection->actor->state;
    placementState = (RoomPlacementState *)&selection->callback;
    if (actorState != 0) {
        actorState->action_state[0] = 4;
    }

    *selection->signal = 0;
    selection->status = 4;
    if (selection->activated != 0) {
        target = *((void **)g_PlayerEntity);
        if (target != 0) {
            if (*(s16 *)&target[0xC] > 0) {
                Battle_ReturnToIdle();
            }
        }
        RoomM273_ReturnPlayerToPlacement((RoomPlacementOwner *)selection->actor, placementState);
    }

    return 0;
}

int RoomM273_GrabNop6(void) {
    return 0;
}

int RoomM273_OscillationNop0(void) {
    return 0;
}

int RoomM273_OscillationInit(RoomOscillationEffect *obj) {
    obj->params.angleOut = 0;
    obj->params.distanceOut = 0;
    obj->params.signal = 0;
    obj->params.active = 0;
    return 0;
}

int RoomM273_OscillationConfigure(RoomOscillationEffect *base, int unused, unsigned int operation, int arg3,
                  int arg4, int arg5) {
    RoomOscillationParams *fields = &base->params;

    switch (operation) {
    case 0:
        fields->active = arg3;
        if (arg4 != 0) {
            fields->frame = 0;
        }
        break;
    case 1:
        fields->amplitude = arg3;
        fields->period = arg4;
        fields->baseHeight = arg5;
        break;
    case 2: {
        FieldActor *owner = base->actor;
        int dx = (*(s32 *)arg3 - owner->pos_x) >> 16;
        int dy = (*(s32 *)arg4 - owner->pos_z) >> 16;

        *(s32 *)arg5 = Math_IntSqrt(dx * dx + dy * dy);
        break;
    }
    case 3:
        *(s32 *)arg4 = RoomM273_ChooseWeightedAction(*(s32 *)arg3, (RoomLibertyChoiceInputs *)fields);
        break;
    case 4:
        fields->angleOut = (int *)arg3;
        fields->distanceOut = (int *)arg4;
        break;
    case 5:
        fields->signal = (int *)arg3;
        *(s32 *)arg3 = 1;
        break;
    }
    return 0;
}

int Math_IntSqrt(int squaredDistance);

int RoomM273_OscillationNop3(void) {
    return 0;
}

int RoomM273_OscillationUpdate(RoomOscillationEffect *effect) {
    int **outs = &effect->params.angleOut;
    FieldActor *actor = effect->actor;

    if (effect->params.active) {
        int frame = effect->params.frame;
        int period = effect->params.period;
        int phase = ((frame << 12) / period - 2048) & 4095;
        register int sine;
        register int half;
        FieldActor *target;

        effect->params.frame = frame + 1;
        sine = rcossin_tbl[phase].part.cos;
        half = (sine + 4096) / 2;
        sine = (half * effect->params.amplitude) / 4096 + 8;
        target = (effect)->actor;
        {
            int height;

            half = sine << 12;
            height = effect->params.baseHeight - half;
            target->pos_y = height;
        }
    }

    if (outs[0]) {
        FieldActor *player = g_PlayerEntity;
        int angle = ratan2(actor->pos_x - player->pos_x,
                                  actor->pos_z - player->pos_z);

        *outs[0] = (angle - actor->rot_y) & 4095;
    }

    if (outs[1]) {
        FieldActor *player = g_PlayerEntity;
        int dx = (player->pos_x - actor->pos_x) >> 16;
        int dz = (player->pos_z - actor->pos_z) >> 16;

        *outs[1] = Math_IntSqrt(dx * dx + dz * dz);
    }

    return 0;
}

int RoomM273_OscillationRelease(RoomOscillationEffect *obj) {
    if (obj->params.signal != 0) {
        obj->params.signal = 0;
    }

    obj->status = 4;
    return 0;
}

int RoomM273_OscillationNop6(void) {
    return 0;
}

s32 RoomM273_ChooseWeightedAction(s32 arg0, RoomLibertyChoiceInputs *inputs) {
    s32 variant;
    s32 distanceBand;
    s32 threshold;
    s32 i;
    s32 sum;
    u8 *weights;

    variant = (*inputs->variantBits >> 10) & 3;
    distanceBand = *inputs->distance;

    if (distanceBand < 0x280) {
        distanceBand = 0;
    } else if (distanceBand < 0x600) {
        distanceBand = 1;
    } else {
        distanceBand = 2;
    }

    weights = g_RoomM273ActionWeights + (arg0 * 84) + (distanceBand * 28) + (variant * 7);
    threshold = ((Inv_ScrambleGrid() & 0xFF) * 100) / 0x100;

    i = 0;
    sum = 0;
    while (i < 6) {
        sum += weights[i];
        if (threshold < sum) {
            return i;
        }
        i++;
    }

    return i;
}

int RoomM273_GrabPlacementFanCallback(int mode, RoomM273SampledLayerEffect *effect) {
    RoomM273Vector position, rotation;
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 8) return 1;
    } else if (mode == 2) {
        short frame = g_FieldFxCurrentAge - 1;
        int offset = ((unsigned int)frame << 9) & 0x3e00;
        int product = ((short *)rcossin_tbl)[offset / 2] * 2 + 4096;
        int sample = ((short *)rcossin_tbl)[offset / 2 + 1];
        short size = product;
        short shade = (unsigned int)sample >> 5;
        short i;
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = frame * 170;
        rotation.pad = -1;
        position = effect->position;
        i = 0;
        do {
            FieldFx_DrawStarFan(&position, 128, 128, 12, (GteRotation *)&rotation,
                          size, size, (RenderColor *)g_RoomM273BlackColor,
                          (RenderColor *)effect->parameter, shade, 1);
            shade = shade >> 1;
            size = size * 3 / 2;
            i++;
            position.y = g_RoomFloorY->raw;
        } while (i < 2);
    }
    return 0;
}

int RoomM273_GrabPlacementFanEmitter(int mode) {
    switch(mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,12,9,
                             (FieldFxCallbackListCallback)RoomM273_GrabPlacementFanCallback);
    case 1: {
        RoomM273SampledLayerEffect *effect;
        RoomPlacementMap *transform;
        if(g_FieldFxCurrentAge>=12) return 2;
        effect=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!effect) break;
        transform=ANIM_OWNER_AS(RoomPlacementStateContext)->owner->map;
        effect->position.x=transform->x;
        asm("" : : : "memory");
        effect->position.y=transform->y;
        effect->position.z=transform->z;
        effect->parameter[0]=((short *)rcossin_tbl)[((g_FieldFxCurrentAge*1024/12)&0xFFF)*2]*40/4096;
        effect->parameter[1]=8;
        {
            unsigned long address=((g_FieldFxCurrentAge*1024/12)&0xFFF)*4;
            address+=(unsigned int)((RoomM273TrigEntry *)rcossin_tbl);
            effect->parameter[2]=((short *)address)[1]*40/4096;
        }
        break;
    }
    case 2:
        break;
    }
    return 0;
}

/* The emitter owns the twelve-byte records rendered by this callback. */

int RoomM273_GrabPlacementScaledFanCallback(int mode,RoomM273ScaledLayerEffect *effect) {
    Vector position,rotation;
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    if(mode==1) {
        if(g_FieldFxCurrentAge>=8) return 1;
    } else if(mode==2) {
        short frame=g_FieldFxCurrentAge-1;
        /* The input vector's fourth halfword supplies the initial scale. */
        int product=(rcossin_tbl[((unsigned int)frame<<7)&0xF80].part.sin*2+4096)*effect->position.pad;
        int sample=rcossin_tbl[((unsigned int)frame<<7)&0xF80].word;
        short size=product/4096;
        short shade=sample>>21;
        short i;
        rotation.x=1024; rotation.y=0; rotation.z=frame*170; rotation.pad=-1;
        position=effect->position;
        i=0;
        do {
            FieldFx_DrawStarFan(&position,128,128,12,(GteRotation *)&rotation,size,size,(struct RenderColor *)g_RoomM273BlackColor,(struct RenderColor *)effect->parameter,shade,1);
            shade>>=1;
            size=size*3/2;
            i++;
            position.y=g_RoomFloorY->raw;
        } while(i<2);
    }
    return 0;
}

int RoomM273_GrabPlacementScaledFanEmitter(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,12,9,
                             (FieldFxCallbackListCallback)RoomM273_GrabPlacementScaledFanCallback);
    case 1: {
        RoomM273ScaledLayerEffect *effect;
        Transform *transform;
        int scale;
        if(g_FieldFxCurrentAge>=12) return 2;
        effect=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!effect) break;
        transform=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.scaled_layer->transform;
        effect->position.x=transform->x;
        asm("" : : : "memory");
        effect->position.y=transform->y;
        effect->position.z=transform->z;
        scale=g_RcossinTblCos[((g_FieldFxCurrentAge*1024/12)&0xFFF)*2];
        effect->parameter[0]=40;
        effect->position.pad=scale;
        effect->parameter[1]=((short *)rcossin_tbl)[((g_FieldFxCurrentAge*1024/12)&0xFFF)*2]*40/4096;
        effect->parameter[2]=0;
        break;
    }
    case 2:
        break;
    }
    return 0;
}

int RoomM273_GrabPlayerPulseCallback(int mode, RoomM273SpritePoolEffect *state) {
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (mode == 2) {
        int index = ((g_FieldFxCurrentAge - 1) << 8) & 0x3f00;
        Sprite sprite;
        int product;
        int sample;
        short size;
        short shade;
        RoomM273WorldPosition *p = state->position;
        int kind;
        int palette;
        int x;

        product = ((short *)rcossin_tbl)[index / 2] * 2 + 4096;
        size = product;
        sample = ((short *)rcossin_tbl)[index / 2 + 1];
        shade = (unsigned int)sample >> 5;
        x = p->x;
        sprite.y = p->y;
        sprite.z = p->z;
        sprite.x = x;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        FieldFx_DrawShapeQuads((GteShortVector *)&sprite, &g_RoomM273FloorRotation, size, size, 102,
                        GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 9 : palette + 5), 1, shade, 0);
    }
    return 0;
}

int RoomM273_GrabPlayerPulseEmitter(int mode) {
    switch (mode) {
    case 0:
        g_RoomM273PlayerPulseSpawned = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 4, 1,
                             (FieldFxCallbackListCallback)RoomM273_GrabPlayerPulseCallback);
    case 1:
        if (g_FieldFxCurrentAge >= 20) return 2;
        if (g_RoomM273PlayerPulseSpawned) return 2;
        if (g_PlayerEntity->mode < 4) {
            RoomM273SpritePoolEffect *effect =
                FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if (effect) {
                RoomM273PlayerTransform *transform = (RoomM273PlayerTransform *)g_PlayerEntity->render_object.matrices;
                g_RoomM273PlayerPulseSpawned = 1;
                effect->position = &transform->position;
            }
        }
        break;
    case 2:
        if (g_RoomM273PlayerPulseSpawned) {
            int index = g_FieldFxTpageSlot11;
            int palette;
            g_FieldFxSpriteParams.parameter00 = 32;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 32;
            g_FieldFxSpriteParams.extent_y = 32;
            g_FieldFxSpriteParams.extent_x = 32;
            g_FieldFxSpriteParams.extent_y = 32;
            palette = g_FieldFxTpageTable[index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0;
            g_FieldFxSpriteParams.tpage = palette;
        }
        break;
    }
    return 0;
}

void *Asset_FindTable08ByU32Key(void *base, s32 key);

int RoomM273_GrabModelRingCallback(int mode, ThresholdPairBand **arg1) {
    short params[4];
    short i;
    ThresholdPairBand **objp = arg1;
    short *x;
    short *x_next;
    int *z;
    int *z_next;

    if (mode == 1) {
        if (g_FieldFxCurrentAge < 0x20) {
            goto ret0;
        }
        return 1;
    }
    if (mode != 2) {
        return 0;
    }

    i = 0;
    x = g_RoomM273GrabModelRingX;
    x_next = x + 1;
    z = g_RoomM273GrabModelRingZ;
    z_next = z + 1;

    params[0] = 0x400;
    params[1] = g_FieldFxCurrentAge << 7;
    params[2] = 0;
    params[3] = 1;

    do {
        FieldFx_DrawRingBand(&(*objp)->position, x[i], x_next[i], 0x10, (GteRotation *)params,
                      (*objp)->scale, (*objp)->scale,
                      (RenderColor *)((((int)i * 4) + (int)z)), (RenderColor *)((((int)i * 4) + (int)z_next)),
                      (*objp)->intensity, 1);
        i++;
    } while (i < 2);

ret0:
    return 0;
ret1:
    return 1;
}

int RoomM273_GrabModelCallback(int mode,ThresholdTripleEffect *effect) {
    ThresholdTripleMatrix matrix;
    ThresholdTripleVector copy,scale;
    if(mode==1) {
        int frame=g_FieldFxCurrentAge;
        int *wave;
        register int *shifted asm("$4");
        register int *table asm("$6");
        unsigned int index;
        if(frame>=32) return 1;
        frame*=32;
        index=frame&0xFFF;
        asm("" : : "r"(index) : "$6");
        table=(int *)rcossin_tbl;
        wave=&table[index];
        shifted=&table[(frame+1024)&0xFFF];
        {
            register int sample asm("$2")=*wave;
            effect->scaleXZ=(short)sample/2;
        }
        effect->scaleY=(((short *)shifted)[1]+4096)*2;
        asm("" : : : "memory");
        {
            unsigned int phase=(unsigned short)effect->phase;
            int parameter=((short *)wave)[1];
            asm("" : "=r"(phase), "=r"(parameter) : "0"(phase), "1"(parameter));
            effect->phase=phase+256;
            effect->parameter12=parameter>>6;
        }
    } else if(mode==2) {
        u16 flags=GetTPage(0,1,0,0);
        int page=(unsigned short)(g_FieldFxTpageTable[g_FieldFxTpageSlot11]|flags);
        int kind=g_FieldFxSpriteParams.palette,palette;
        int cosine,sine;
        register void **asset asm("$17");
        palette=g_FieldFxPaletteTable[kind];
        FieldFx_ModelSetTexture(page,GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 11 : palette + 7));
        FieldFx_ModelSetSemiTrans(1);
        asset=&g_RoomM273GrabModel.asset;
        FieldFx_ModelSaveColors(*asset);
        FieldFx_ModelScaleColors(*asset,effect->parameter12);
        cosine=rcos(effect->phase);
        sine=rsin(effect->phase);
        matrix.m[0][2]=sine; matrix.m[2][0]=-sine;
        matrix.m[0][0]=cosine; matrix.m[2][2]=cosine;
        matrix.t[0]=matrix.t[1]=matrix.t[2]=0;
        matrix.m[0][1]=matrix.m[1][0]=matrix.m[1][2]=matrix.m[2][1]=0;
        matrix.m[1][1]=4096;
        memset(&scale,0,16);
        scale.x=effect->scaleXZ; scale.y=effect->scaleY; scale.z=effect->scaleXZ;
        copy=scale;
        ScaleMatrix(&matrix,&copy);
        {
            int x=effect->x;
            void *model=*asset;
            matrix.t[0]=x; matrix.t[1]=effect->y; matrix.t[2]=effect->z;
            FieldFx_ModelDraw(model,&matrix);
        }
        FieldFx_ModelRestoreColors(*asset);
    }
    return 0;
}

int RoomM273_GrabModelEmitter(int mode) {
    switch(mode) {
    case 0: {
        int size;
        g_RoomM273GrabModel.asset=Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0],0xC5941704);
        FieldFx_ModelScrollUv(g_RoomM273GrabModel.asset,0,0);
        size=FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,16,6,
                           (FieldFxCallbackListCallback)RoomM273_GrabModelCallback);
        asm("" : : "r"(size));
        return size+FieldFx_AllocCallbackList(&g_RoomM273GrabModel.pool,size,4,2,RoomM273_GrabModelRingCallback);
    }
    case 1: {
        RoomM273ThresholdEntityState *state;
        if(g_RoomM273GrabEffectStopped) return 2;
        state=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.threshold;
        if(state->mode.kind==9) {
            unsigned short value=state->mode.value26;
            if(state->mode.value22>0 && (short)value<=0) {
                short i=0;
                do {
                    ThresholdTripleEffect *effect=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    if(!effect) break;
                    effect->x=state->transform->x;
                    effect->y=g_RoomFloorY->raw;
                    effect->z=state->transform->z;
                    effect->phase=i*384;
                    if(i==0) *(ThresholdTripleEffect **)FieldFx_AcquireCallbackSlot(g_RoomM273GrabModelList)=effect;
                    i++;
                } while(i<3);
            }
        }
        FieldFx_UpdateCallbackList(g_RoomM273GrabModelList);
        break;
    }
    case 2:
        FieldFx_DrawCallbackList(g_RoomM273GrabModelList);
        break;
    }
    return 0;
}

int RoomM273_GrabPlayerStreakCallback(int mode, GteRotation *rotation) {
    GteShortVector position;
    int frame, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (mode == 2) {
        RoomM273PlayerActorView *player;
        /* The loop index is reused for the sampled scale below. */
        size = 0;
        player = (RoomM273PlayerActorView *)g_PlayerEntity;
        for (; size < 3; size++) {
            RoomM273PlayerTransform *transform = player->tail.transform;
            ((s16 *)&position)[size] = ((s32 *)&transform->position.x)[size];
        }

        frame = g_FieldFxCurrentAge - 1;
        kind = g_FieldFxSpriteParams.palette;
        size = rcossin_tbl[(frame << 6) & 0xFC0].part.sin;
        palette = g_FieldFxPaletteTable[kind];

        clut = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 11 : palette + 7);
        FieldFx_DrawShapeQuads(&position, rotation, 8192, size * 3, 5, clut, 1,
                       /* The upper half of the packed trig entry is signed. */
                       (s16)(rcossin_tbl[(frame << 6) & 0xFC0].word >> 16) >> 5, 0);
    }
    return 0;
}

int RoomM273_GrabPlayerStreakEmitter(int mode) {
    switch (mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 4,
                             (FieldFxCallbackListCallback)RoomM273_GrabPlayerStreakCallback);
    case 1: {
        RoomM273EffectModeState *state;
        if (g_RoomM273GrabEffectStopped) return 2;
        state = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 > 0 && (short)value <= 0) {
                short i = 0;
                int angle = Inv_ScrambleGrid() << 4;
                do {
                    RoomM273PulseSeed *seed = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    if (!seed) break;
                    *seed = g_RoomM273PlayerStreakSeeds[i];
                    i++;
                    seed->angle += angle;
                } while (i < 2);
            }
        }
        break;
    }
    case 2: {
        int index;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.extent_x = 16; g_FieldFxSpriteParams.extent_y = 16; g_FieldFxSpriteParams.extent_x = 16;
        index = g_FieldFxTpageSlot11;
        g_FieldFxSpriteParams.parameter02 = 1; g_FieldFxSpriteParams.extent_y = 64;
        palette = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 3; g_FieldFxSpriteParams.parameter06 = 1; g_FieldFxSpriteParams.parameter0A = 3; g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
    }
    return 0;
}

int RoomM273_GrabPlayerTrailCallback(int mode, Vector *input) {
    Vector *position = input;
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 9) return 1;
    } else if (mode == 2) {
        int firstFrame = g_FieldFxCurrentAge - 1;
        int frame;
        unsigned int offset;
        int kind = g_FieldFxSpriteParams.palette;
        s16 size;
        int palette;
        unsigned short clut;

        offset = ((unsigned int)firstFrame << 9) & 0x3E00;
        frame = firstFrame;
        size = rcossin_tbl[offset / 4].part.sin * 2 + 4096;

        palette = g_FieldFxPaletteTable[kind];
        clut = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 9 : palette + 5);
        FieldFx_DrawShapeQuads(position, 0, size, size, 102, clut, 1,
            (short)rcossin_tbl[((unsigned int)frame << 8) & 0xF00].word >> 6, 0);
    }
    return 0;
}

int RoomM273_GrabPlayerTrailEmitter(int mode) {
    switch(mode) {
    case 0:
        g_RoomM273PlayerTrailBurstsLeft=0; g_RoomM273PlayerTrailDelay=0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,4,
                             (FieldFxCallbackListCallback)RoomM273_GrabPlayerTrailCallback);
    case 1: {
        RoomM273EffectModeState *state;
        if(g_RoomM273GrabEffectStopped) return 2;
        state=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if(state->kind==9) {
            unsigned short value=state->value26;
            if(state->value22>=2 && (short)value<2) {
                g_RoomM273PlayerTrailBurstsLeft=4; g_RoomM273PlayerTrailDelay=0;
            }
        }
        if(g_RoomM273PlayerTrailBurstsLeft) {
            short old=g_RoomM273PlayerTrailDelay--;
            if(old<=0) {
                Vector *position=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if(position) {
                    RoomM273PlayerTransform *transform=(RoomM273PlayerTransform *)g_PlayerEntity->render_object.matrices;
                    position->x=transform->position.x;
                    position->y=transform->position.y;
                    position->z=transform->position.z;
                    g_RoomM273PlayerTrailDelay=2;
                    g_RoomM273PlayerTrailBurstsLeft--;
                }
            }
        }
        break;
    }
    case 2: {
        int index=g_FieldFxTpageSlot11;
        int palette;
        g_FieldFxSpriteParams.parameter00=32; g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32; g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=3; g_FieldFxSpriteParams.parameter06=1; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=32;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

int RoomM273_GrabStarBurstCallback(int mode, RoomM273PaletteInput *input)
{
    RoomM273TrigEntry *entry;
    RoomM273PaletteEffect *effect;
    GteShortVector position;
    GteVector *source;
    int x;
    int y;
    int z;
    GteRotation rotation;
    int random;

    if (mode == 1) {
        int frame;
        frame = g_FieldFxCurrentAge;
        if (frame >= 33) return 1;
        entry = &rcossin_tbl[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        input->size = (entry->low * 3 * 2048) / 4096 + 4096;
        if (frame >= 24) return 0;
        effect = FieldFx_AcquireCallbackSlot(g_RoomM273GrabSparkPool);
        if (!effect) return 0;
        effect->source = input->source;
        effect->size = (input->size * 3 * 256) / 4096;
        effect->depth = Inv_ScrambleGrid() + 256;
        effect->color = g_RoomM273StarBurstColors[g_FieldFxCurrentAge & 3];
        effect->x = Inv_ScrambleGrid() << 4;
        random = Inv_ScrambleGrid();
        effect->y = (128 - random) >> 2;
    } else if (mode == 2) {
        int frame;
        source = input->source;
        frame = g_FieldFxCurrentAge;
        y = source->y;
        x = source->x;
        frame = frame - 1;
        position.y = y;
        z = source->z;
        entry = &rcossin_tbl[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        position.z = z;
        position.x = x;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = 170 * frame;
        rotation.flags = 0;
        FieldFx_DrawStarFan(&position, 256, 256, 12, &rotation,
                       input->size, input->size,
                       (RenderColor *)g_RoomM273BlackColor,
                       (RenderColor *)&g_RoomM273GrabColors[g_RoomM273GrabColorIndex],
                       entry->high >> 5, 1);
    }
    return 0;
}

int RoomM273_GrabStarBurstEmitter(int mode) {
    switch (mode) {
    case 0: {
        int size = FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 2,
                                 (FieldFxCallbackListCallback)RoomM273_GrabStarBurstCallback);
        asm("" : : "r"(size));
        return size + FieldFx_AllocCallbackList(&g_RoomM273GrabSparkPool, size, 16, 9, RoomM273_GrabSparkTriangleCallback);
    }
    case 1: {
        RoomM273EffectModeState *state;
        if (g_RoomM273GrabEffectStopped) return 2;
        state = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 >= 4 && (short)value < 4) {
                RoomM273PointerPoolEffect *effect = (RoomM273PointerPoolEffect *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if (effect) effect->position = g_PlayerEntity->render_object.matrices->translation;
            }
        }
        FieldFx_UpdateCallbackList(g_RoomM273GrabSparkPool);
        break;
    }
    case 2:
        FieldFx_DrawCallbackList(g_RoomM273GrabSparkPool);
        break;
    }
    return 0;
}

int RoomM273_GrabHoldFlareCallback(int mode, GteShortVector *input)
{
    GteRotation rotation;
    GteShortVector position;
    int frame;
    RoomM273TrigEntry *entry;
    int size;
    s16 *indexPtr;
    RenderColor *sprite;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (mode == 2) {
        frame = (s16)(g_FieldFxCurrentAge - 1);
        entry = &rcossin_tbl[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)];
        size = (entry->low * 5 * 2048) / 4096 + 2048;
        position = *input;
        indexPtr = &g_RoomM273GrabColorIndex;
        sprite = (RenderColor *)g_RoomM273BlackColor;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = frame * 204;
        rotation.flags = 0;
        FieldFx_DrawStarFan(&position, 160, 160, 10, &rotation, (s16)size, (s16)size,
                       sprite, (RenderColor *)&g_RoomM273GrabColors[*indexPtr], entry->high >> 5, 1);

        position.y = g_RoomFloorY->raw;
        rotation.x = 1024;
        rotation.flags = 1;
        FieldFx_DrawStarFan(&position, 256, 256, 10, &rotation,
                       (s16)size >> 1, (s16)size >> 1,
                       (RenderColor *)&g_RoomM273GrabColors[*indexPtr], sprite, entry->high >> 5, 1);
    }
    return 0;
}

int RoomM273_GrabEffectController(int mode) {
    FieldActor *obj;
    unsigned char *state;
    unsigned char obj_mode;
    unsigned short frame;
    unsigned short timer;
    RenderMatrix *data;

    if (mode != 1) {
        if (mode >= 2) {
            return 0;
        }
        if (mode != 0) {
            return 0;
        }
        g_RoomM273GrabEffectStopped = 0;

    }
    else
    {
mode_one:
    obj = g_FieldFxOwner->actor;
    state = obj->state->action_state;
    frame = obj->anim.parts.integer;
    /* High half of anim_prev, the previous whole frame. */
    timer = *(unsigned short *)&((char *)obj)[0x1A];

    if (*state == 1) {
        *state = 2;
    }

    obj_mode = obj->mode;
    if (obj_mode != 9) {
        if (obj_mode == 0x10) {
            if ((unsigned int)(frame - 0x23) < 0x19) {
                g_RoomM273GrabEffectStopped = 1;
            }
        }
    } else if ((short)frame >= 0x21) {
        g_RoomM273GrabEffectStopped = 1;
    } else if ((short)frame > 0 && ((timer << 16) <= 0)) {
        data = obj->render_object.matrices;
        Asset_Find08w(0x5CF, obj->state->progress,
                      (short)data[44].translation[0], (short)data[44].translation[1],
                      (short)data[44].translation[2]);
    }

    if (g_RoomM273GrabEffectStopped != 0) {
        return 1;
    }

    }
ret_zero:
    return 0;

ret_one:
    return 1;
}

/* Falling trail record: spins down from the boss, records where it lands or
 * touches the player, and draws a fading two-point trail ring behind it. */
int RoomM273_HandTrailCallback(int mode, RoomM273FallingTrail *trail) {
    GteMatrix matrix;
    GteShortVector vector;
    GteShortVector point;
    GteRotation rotation;
    s16 *source;
    int i;
    s16 floor;
    s16 intensity;
    s16 scale;

    if (mode == 1) {
        if (trail->landed) {
            return --trail->count <= 0;
        }
        if (trail->spin) {
            trail->yaw += trail->spin;
            trail->spin += trail->spin < 0 ? -2 : 2;
        }
        vector.x = trail->pitch;
        vector.y = trail->yaw;
        vector.z = 0;
        RotMatrixYXZ(&vector, &matrix);
        for (i = 0, source = &trail->position.x; i < 3; i++) {
            matrix.t[i] = *source++;
        }
        gte_ldrotmatrix(&matrix);
        gte_ldtransmatrix(&matrix);
        gte_ldv0(&g_RoomM273HandTrailDropOffset);
        gte_rtv0tr_mac();
        gte_stsv(&trail->position);
        floor = g_RoomFloorY->raw;
        if (trail->position.y >= floor - 0x80) {
            trail->position.y = floor;
            trail->landed = 1;
        }
        if (trail->position.y >= g_RoomM273HandTrailState.floor && g_PlayerEntity->mode >= 4) {
            int dx = g_PlayerEntity->render_object.model_matrix.translation[0] - trail->position.x;
            int dz = g_PlayerEntity->render_object.model_matrix.translation[2] - trail->position.z;
            if (Math_IntSqrt(dx * dx + dz * dz) < 0x140) {
                trail->landed = 1;
                source = &trail->position.x;
                for (i = 0; i < 3; i++) {
                    g_RoomM273HandTrailState.hit[i] = (g_PlayerEntity->render_object.matrices->translation[i] + source[i]) / 2;
                }
                g_RoomM273HandTrailState.hit_flag = 1;
                g_PlayerEntity->state->flags |= 0x4000;
                if (ANIM_OWNER_AS(RoomM273BossActor)->instance->owner) {
                    ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->flags |= 0x80000000;
                }
            }
        }
        if (trail->landed) {
            i = g_RoomM273HandTrailState.landing_count++;
            g_RoomM273HandTrailState.landing_x[i] = trail->position.x;
            g_RoomM273HandTrailState.landing_y[i] = trail->position.y;
            g_RoomM273HandTrailState.landing_z[i] = trail->position.z;
        }
        trail->head = (trail->head - 1) & 7; i = trail->head;
        gte_ldv0(&g_RoomM273HandTrailSampleOffsetA);
        gte_rtv0tr_mac();
        gte_stsv(&vector);
        trail->trail_y[i] = vector.y;
        i *= 2;
        trail->trail_x[i] = vector.x;
        trail->trail_z[i] = vector.z;
        gte_ldv0(&g_RoomM273HandTrailSampleOffsetB);
        gte_rtv0tr_mac();
        gte_stsv(&vector);
        i++;
        trail->trail_x[i] = vector.x;
        trail->trail_z[i] = vector.z;
        if (trail->count < 8) {
            trail->count++;
        }
    } else if (mode == 2) {
        if (!trail->landed) {
            int kind;
            int palette;
            int index = TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot3);

            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            rotation.x = trail->pitch + 0x400;
            rotation.y = trail->yaw;
            rotation.z = -0x400;
            rotation.flags = 1;
            if (g_FieldFxCurrentAge < 17) {
                scale = rcossin_tbl[(g_FieldFxCurrentAge << 8 & 0x3F00) >> 2].bits.sin * 3 * 2048 / 4096 + 0x800;
            } else {
                scale = 0x2000;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) {
                palette += 4;
            }
            FieldFx_DrawShapeQuads(&trail->position, &rotation, scale, 0x1000, 0xC,
                          GetClut(0x30, palette), 1, 0x80, 0);
            intensity = 0x80;
        } else {
            intensity = trail->count * 16;
        }
        scale = (0x80 - intensity) * 64 + 0x1000;
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 8;
        rotation.flags = 0;
        {
            s16 step;
            s16 index = trail->head;

            for (step = 0; step < trail->count; step++, index = (index + 1) & 7) {
                s16 side;

                point.y = trail->trail_y[index];
                for (side = 0; side < 2; side++) {
                    int kind;
                    int palette;

                    point.x = trail->trail_x[index * 2 + side];
                    point.z = trail->trail_z[index * 2 + side];
                    kind = g_FieldFxSpriteParams.palette;
                    palette = g_FieldFxPaletteTable[kind];
                    if (kind == 4 && g_FieldFxAltPaletteFlag) {
                        palette += 4;
                    }
                    FieldFx_DrawShapeQuads(&point, &rotation, scale, scale, 0xDC,
                                  GetClut(0x30, palette), 1, intensity, &g_RoomM273HandTrailColor);
                    rotation.y += 0x800;
                }
                intensity -= 0x10;
                scale += 0x400;
                rotation.z += 0x100;
            }
        }
    }
    return 0;
}

int RoomM273_HandTrailEmitter(int mode) {
    s16 nextIndex;
    FieldActor *actor;
    FieldActor *candidate;
    s16 size;
    s16 i;
    s32 pulse;
    s16 *pathAddress;
    s32 pathOffset;

    switch (mode) {
    case 0: {
        g_RoomM273HandTrailPhase = 0;
        g_RoomM273HandTrailCooldown = 0;
        g_RoomM273HandTrailPeriodA = (Inv_ScrambleGrid() & 7) + 4;
        g_RoomM273HandTrailPeriodB = (Inv_ScrambleGrid() & 7) + 4;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 100, 10,
                             (FieldFxCallbackListCallback)RoomM273_HandTrailCallback);
    }
    case 1: {
        s16 pathY[2];
        RenderMatrix *matrices;

        if (g_RoomM273HandTrailState.done)
            return 2;
        candidate = ANIM_OWNER_AS(RoomM273RingController)->actor;
        if (candidate->mode != 11)
            return 0;
        actor = candidate;
        {
            u16 previousFrame =
                *(u16 *)&((u8 *)actor)[0x1A];
        if ((s16)actor->anim.parts.integer < 6 ||
            (s16)previousFrame >= 35)
            return 0;
        }
        {
            s16 countdown = g_RoomM273HandTrailCooldown;
            g_RoomM273HandTrailCooldown = countdown - 1;
            if (countdown > 0)
                return 0;
        }

        if (g_RoomM273HandTrailState.spinning == 0)
            goto deterministicPattern;
        if (g_RoomM273HandTrailState.spinning == mode)
            goto randomPattern;
        goto phasePattern;

deterministicPattern: {
            register s16 phase asm("$2") = g_RoomM273HandTrailPhase;
            s16 offset;
            pulse = (u16)phase << 16;
            offset = (pulse >> 18) * 32 - 0x300;
            phase &= 3;
            asm("");
            pathY[1] = offset;
            pathY[0] = offset;
            pulse = (pulse >> 15) + 0x80;
            g_RoomM273HandTrailPulse = pulse;
            size = phase << 6;
            goto particleSetup;
        }
randomPattern: {
            int random = Inv_ScrambleGrid() & 0x3F;
            int center = 0x20 - random;
                if (g_RoomM273HandTrailPhase & 1) {
                    pathY[0] = center - 0x340;
                    pathY[1] = center - 0x2A0;
                } else {
                    pathY[1] = center - 0x2A0;
                    pathY[0] = center - 0x340;
            }
            size = 0x10;
            g_RoomM273HandTrailPulse = 0x80;
            goto particleSetup;
        }
phasePattern: {
            u16 phase = g_RoomM273HandTrailPhase;
            s16 offset;
            offset = (phase & 3) * 24;
                if (phase & 1) {
                    s16 pathValue;
                    pathValue = offset - 0x3C0;
                    pathY[0] = pathValue;
                    pathValue = offset - 0x2A0;
                    pathY[1] = pathValue;
                } else {
                    s16 pathValue;
                    pathValue = offset - 0x2A0;
                    pathY[1] = pathValue;
                    pathValue = offset - 0x3C0;
                    pathY[0] = pathValue;
            }
            size = 0x20;
            g_RoomM273HandTrailPulse = 0x80;
        }

particleSetup:
        i = 0;
        do {
            RoomM273FallingTrail *particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if (particle == 0)
                break;
            particle->position.x = g_RoomM273HandTrailState.hands[i].x;
            particle->position.y = g_RoomM273HandTrailState.hands[i].y;
            pathOffset = i * 2;
            particle->position.z = g_RoomM273HandTrailState.hands[i].z;
            particle->yaw = actor->rot_y + size * 4;
            pathAddress = (s16 *)&((u8 *)pathY)[pathOffset];
            nextIndex = i + 1;
            particle->pitch = *pathAddress;
            i = nextIndex;
            particle->spin = size;
            particle->landed = 0;
            particle->head = 0;
            particle->count = 0;
            size = -size;
        } while (nextIndex < 2);

        g_RoomM273HandTrailCooldown = 6;
        g_RoomM273HandTrailPhase++;
        matrices = actor->render_object.matrices;
        Asset_Find08w(0x5D0, ((int *)actor->state)[2],
                      ((s16 *)matrices[2].translation)[0],
                      ((s16 *)matrices[2].translation)[2],
                      ((s16 *)matrices[2].translation)[4]);
        return 0;
    }
    case 2: {
        u16 drawSize = g_FieldFxSpriteParams.parameter00;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.extent_x = drawSize;
        g_FieldFxSpriteParams.extent_y = drawSize;
        break;
    }
    }
    return 0;
}

int RoomM273_HandTrailLandingPulseCallback(int mode, RoomM273PulseInput *input)
{
    GteShortVector position;
    int frame;
    int palette;
    int size;
    int sample;
    u16 clut;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        input->position.pad += input->velocity;
        input->velocity += 2;
    } else if (mode == 2) {
        frame = g_FieldFxCurrentAge - 1;
        sample = rcossin_tbl[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)].bits.sin;
        position = input->position;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        size = sample * 2 + 2048;
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(16, palette);
        FieldFx_DrawShapeQuads(&position, 0, (s16)size, (s16)size,
                       (s16)g_FieldFxSpriteParams.parameter02 * (frame / 2) + 128, clut, 1,
                       rcossin_tbl[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)].bits.cos >> 5, 0);
        position.y -= input->position.pad;
        {
            int clut2;
            palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
            if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            clut2 = GetClut(64, palette);
            FieldFx_DrawShapeQuads(&position, 0, 8192, 8192,
                           (s16)g_FieldFxSpriteParams.parameter02 * g_RoomM273HandTrailLandingPulseScale[frame / 4], clut2, 1,
                           (((s32 *)rcossin_tbl)[(((unsigned int)frame << 9) & 0x3E00) / 4] << 16) >> 21, 0);
        }
    }
    return 0;
}

int RoomM273_HandTrailLandingEmitter(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0: {
        int size=FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,12,10,
                               (FieldFxCallbackListCallback)RoomM273_HandTrailLandingPulseCallback);
        asm("" : : "r"(size));
        return size+FieldFx_AllocCallbackList(&g_RoomM273HandTrailLandingFanPool,size,4,10,RoomM273_HandTrailLandingFanCallback);
    }
    case 1: {
        int i;
        register unsigned char *stopped asm("$3")=&g_RoomM273HandTrailState.done;
        if(*stopped) {
            FieldFx_UpdateCallbackList(g_RoomM273HandTrailLandingFanPool);
            return 2;
        }
        i=0;
        if(g_RoomM273HandTrailState.landing_count>0) {
            /* The stop symbol is the byte at offset 0x40 of this buffer. */
            register RoomM273PairedPulseBatch *batch asm("$19")=
                (RoomM273PairedPulseBatch *)(stopped-PE1_OFFSETOF(RoomM273PairedPulseBatch, stopped));
            unsigned short *x=batch->x;
            int count;
            do {
                RoomM273PulseEmitterView *effect=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                RoomM273PulseEmitterView **pointer;
                register int offset asm("$4");
                unsigned short y;
                if(!effect) break;
                pointer=FieldFx_AcquireCallbackSlot(g_RoomM273HandTrailLandingFanPool);
                offset=i*2;
                asm("" : : "r"(offset));
                effect->x=*x;
                y=(&((unsigned short *)&((char *)batch)[offset])[8])[0];
                asm("" : "=r"(offset) : "0"(offset));
                effect->y=y-128;
                effect->z=(&((unsigned short *)&((char *)batch)[offset])[16])[0];
                effect->phase=0;
                effect->size=128;
                *pointer=effect;
                count=batch->count;
                asm("" : : "r"(i), "r"(x) : "memory");
                ++i;
                ++x;
            } while(i<count);
        }
        g_RoomM273HandTrailState.landing_count=0;
        FieldFx_UpdateCallbackList(g_RoomM273HandTrailLandingFanPool);
        break;
    }
    case 2: {
        unsigned int index;
        unsigned short palette;
        FieldFx_DrawCallbackList(g_RoomM273HandTrailLandingFanPool);
        index=g_FieldFxTpageSlot3;
        g_FieldFxSpriteParams.parameter00=32; g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=3; g_FieldFxSpriteParams.parameter06=0; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

/* The rising-sprite callback and its poll/reset controller share one TU. */

int RoomM273_HandTrailHitCallback(int mode, Particle *particle) {
    Color color = g_RoomM273HandTrailHitColor;
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        else {
            unsigned short y = particle->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((Particle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = g_FieldFxCurrentAge - 1;
        int sample = rcossin_tbl[((unsigned int)frame << 6) & 0xFC0].part.sin;
        int size, kind, palette;
        register int specialKind asm("$3") = 4;
        unsigned short clut;
        kind = g_FieldFxSpriteParams.palette;
        size = sample * 2;
        asm("" : : "r"(size) : "memory");
        palette = g_FieldFxPaletteTable[kind];
        if (kind == specialKind && g_FieldFxAltPaletteFlag) palette += 9;
        else palette += 5;
        clut = GetClut(0, palette);
        FieldFx_DrawShapeQuads((struct GteShortVector *)particle, 0, size, size, 102, clut, 1,
            (short)rcossin_tbl[(((unsigned int)frame << 9) & 0x3E00) / 4].word >> 6, (struct RenderColor *)&color);
    }
    return 0;
}

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR ANIM_POOL_AS(RoomM273PoolContext)->pool
#define ROOMLIB_POLL_RESET_FUNC RoomM273_HandTrailHitEmitter
#define ROOMLIB_POLL_RESET_CALLBACK RoomM273_HandTrailHitCallback
#define ROOMLIB_POLL_RESET_FLAG g_RoomM273HandTrailState.done
#define ROOMLIB_POLL_RESET_COUNTER g_RoomM273HandTrailState.hit_flag
#define ROOMLIB_POLL_RESET_SEED g_RoomM273HandTrailHitSeed
#include "../room_lib/poll_reset_actor.inc"

typedef RoomM273EffectModeState State;

int RoomM273_HandGlowEmitter(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0: {
        int size=FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,4,6,RoomM273_PulseSpriteCallback);
        asm("" : : "r"(size));
        return size+FieldFx_AllocCallbackList(&g_RoomM273HandRingBandPool,size,4,6,RoomM273_HandRingBandCallback);
    }
    case 1: {
        unsigned char *stopped=&g_RoomM273HandTrailState.done;
        State *state;
        if(*stopped) return 2;
        state=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if(state->kind==11) {
            int count=state->value22;
            unsigned short value=*(volatile unsigned short *)&state->value22;
            if(count<35 && (value&7)==0) {
                short i=0;
                /* The shared positions start 0x5C bytes before the stop symbol. */
                Vector *positions=(Vector *)(stopped-92);
                do {
                    Vector **out=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    unsigned long address;
                    if(!out) break;
                    address=i*8;
                    address+=(unsigned int)positions;
                    *out=(Vector *)address;
                    i++;
                } while(i<2);
                i=0;
                positions=g_RoomM273HandTrailState.hands;
                do {
                    Vector **out=FieldFx_AcquireCallbackSlot(g_RoomM273HandRingBandPool);
                    unsigned long address;
                    if(!out) break;
                    address=i*8;
                    address+=(unsigned int)positions;
                    *out=(Vector *)address;
                    i++;
                } while(i<2);
            }
            FieldFx_UpdateCallbackList(g_RoomM273HandRingBandPool);
        }
        break;
    }
    case 2: {
        unsigned int index;
        unsigned short palette;
        FieldFx_DrawCallbackList(g_RoomM273HandRingBandPool);
        index=g_FieldFxTpageSlot11;
        g_FieldFxSpriteParams.parameter00=32; g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=3; g_FieldFxSpriteParams.parameter06=1; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

#undef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#undef ROOMLIB_POLL_RESET_POOL_EXPR
#undef ROOMLIB_POLL_RESET_FUNC
#undef ROOMLIB_POLL_RESET_CALLBACK
#undef ROOMLIB_POLL_RESET_FLAG
#undef ROOMLIB_POLL_RESET_COUNTER
#undef ROOMLIB_POLL_RESET_SEED
#undef PE1_EXT_B7E658A5B5B4

/* Tracks the boss's two hand positions, turns it toward the player (or by
 * a scripted spin) during animation 11 and bobs its height. */
int RoomM273_HandTrailController(int mode) {
    RoomM273BossInstance *instance = ANIM_OWNER_AS(RoomM273BossActor)->instance;
    int frame;
    int value;
    GteShortVector *hand;

    if (mode == 0) {
        g_RoomM273HandTrailState.done = 0;
        g_RoomM273HandTrailState.landing_count = 0;
        g_RoomM273HandTrailState.hit_flag = 0;
        g_RoomM273HandTrailState.spin = 0;
        g_RoomM273HandTrailState.tick = 0;
        g_RoomM273HandTrailState.floor = g_RoomFloorY->raw - 0x200;
        g_RoomM273HandTrailState.base_height = instance->height;
    } else if (mode == 1) {
        hand = g_RoomM273HandTrailState.hands;
        for (value = 0; value < 2; value++) {
            GteMatrix *m = instance->transforms;
            s32 *t = value ? m[18].t : m[13].t;
            hand[value].x = t[0];
            hand[value].y = t[1];
            hand[value].z = t[2];
        }
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        frame = instance->time.parts.frame;
        if (instance->animation != 11) return 0;
        if ((s16)frame >= instance->frame_count - 1) {
            g_RoomM273HandTrailState.done = 1;
            if (instance->owner) *instance->owner->status = 4;
            return 1;
        }
        if ((s16)frame >= 0x22) {
            if (g_RoomM273HandTrailState.repeat-- > 0) instance->time.fixed = 0x40000;
        }
        if (g_RoomM273HandTrailState.spinning) {
            if (g_RoomM273HandTrailState.spin > g_RoomM273HandTrailState.spin_target) g_RoomM273HandTrailState.spin -= g_RoomM273HandTrailState.spin_step;
            else if (g_RoomM273HandTrailState.spin < g_RoomM273HandTrailState.spin_target) g_RoomM273HandTrailState.spin += g_RoomM273HandTrailState.spin_step;
            instance->yaw = (instance->yaw + g_RoomM273HandTrailState.spin) & 0xFFF;
            if (g_RoomM273HandTrailState.tick >= g_RoomM273HandTrailState.tick_limit) g_RoomM273HandTrailState.spin_target = 0;
        } else {
            /* The heading is turned half way round in 16.16 form. */
            value = FieldFx_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation, (Vec3Fixed *)instance->position);
            instance->yaw = FieldFx_TurnToward(instance->yaw, (short)((value << 16) + 0x8000000 >> 16), 0x20);
        }
        if (g_RoomM273HandTrailState.tick < 0x77) {
            value = (g_RoomM273HandTrailState.tick << 13) / 118;
            value = rcossin_tbl[(value - 0x800) & 0xFFF].bits.cos + 0x1000;
            instance->height = ((-(value * 384) / 8192) << 16) + g_RoomM273HandTrailState.base_height;
        }
        g_RoomM273HandTrailState.tick++;
    }
    return 0;
}

/* Falling drop: queues its landing position, then knocks the player back on
 * contact; draws two rings before landing and a splash flash after. */
int RoomM273_SweepDropCallback(int mode, RoomM273Drop *drop) {
    GteShortVector ring;
    GteShortVector unused;
    int i;

    if (mode == 1) {
        if (drop->position.pad == 0) {
            RoomFloorY *floor = g_RoomFloorY;
            drop->position.x += drop->vx;
            drop->position.y += drop->vy;
            drop->position.z += drop->vz;
            if (drop->position.y >= (s16)floor->raw) {
                drop->position.y = floor->raw;
                drop->position.pad = 1;
                i = g_RoomM273SweepState.count++;
                g_RoomM273SweepState.x[i] = drop->position.x;
                g_RoomM273SweepState.y[i] = floor->raw;
                g_RoomM273SweepState.z[i] = drop->position.z;
            }
            if (drop->touched) return 0;
            if (drop->position.y < g_RoomM273SweepState.floor) return 0;
            if (Math_IntSqrt((g_PlayerEntity->render_object.model_matrix.translation[0] - drop->position.x) *
                                 (g_PlayerEntity->render_object.model_matrix.translation[0] - drop->position.x) +
                             (g_PlayerEntity->render_object.model_matrix.translation[2] - drop->position.z) *
                                 (g_PlayerEntity->render_object.model_matrix.translation[2] - drop->position.z)) < 0x80) {
                drop->touched = 1;
                g_RoomM273SweepState.hit.vector = drop->position;
                g_RoomM273SweepState.hit.vector.pad = 1;
                g_PlayerEntity->state->flags |= 0x4000;
                if (ANIM_OWNER_AS(RoomM273BossActor)->instance->owner)
                    ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->flags |= 0x80000000;
            }
        } else {
            if (drop->position.pad++ >= 8) return 1;
        }
    } else if (mode == 2) {
        int shade = rcossin_tbl[(drop->position.pad << 7) & 0xF80].bits.cos >> 5;
        RoomM273BossTrig *trig = ((RoomM273BossTrig *)rcossin_tbl);
        ring.x = drop->ring.x;
        ring.y = drop->ring.y;
        ring.z = drop->ring.z;
        if (drop->position.pad == 0) {
            for (i = 0; i < 2; i++) {
                FieldFx_DrawRotatedTriangle(&drop->position, (GteRotation *)&ring, 0x100, 0x10, &g_RoomM273SweepDropColorB,
                              &g_RoomM273SweepDropColorA, &g_RoomM273SweepDropColorA, (s16)shade, 1);
                ring.z += 0x400;
            }
        } else {
            int size = trig[(drop->position.pad << 7) & 0xF80].sine * 2 + 0x1000;
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_DrawShapeQuads(&drop->position, &g_RoomM273FloorRotation, size, size, 0xDC,
                          GetClut(0x30, palette), 1, shade, &g_RoomM273SweepDropColorC);
        }
    }
    return 0;
}

/* room_m273, file offset 0x79F0, 1364 retail-matching bytes. */

int RoomM273_SweepDropEmitter(int mode) {
    Vector playerPosition;
    Matrix matrix;
    switch(mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,24,40,
                             (FieldFxCallbackListCallback)RoomM273_SweepDropCallback);
    case 1: {
        register unsigned char *stopped asm("$17")=&g_RoomM273SweepState.done;
        Player *player;
        int dx,dy,dz,x,y,z; short spread; short angle;
        if(*stopped) return 2;
        player=g_PlayerEntity;
        playerPosition.x=x=((Pe1Fixed16_16 *)&player->pos_x)->parts.integer;
        dx=x-g_RoomM273SweepPoint.x;
        playerPosition.y=y=((Pe1Fixed16_16 *)&player->pos_y)->parts.integer;
        playerPosition.z=z=((Pe1Fixed16_16 *)&player->pos_z)->parts.integer;
        dz=z-g_RoomM273SweepPoint.z;
        dy=y-g_RoomM273SweepPoint.y;
        angle=ratan2(dy,Math_IntSqrt((unsigned int)dx*dx+(unsigned int)dz*dz))&4095;
        if(angle>768) angle=768;
        if(angle<384) angle=384;
        /* Pitch is clamped to [384, 768]; the second ring halves its spread. */
        spread=angle-256;
        if(g_RoomM273SweepState.in_contact) {
            short i=0;
            int width=spread;
            /* Retail address window: heading 0x8019AF60, origin 0x8019AEFC. */
            register unsigned short *heading asm("$19")=(unsigned short *)(stopped-9);
            register Matrix *transform asm("$16")=&matrix;
            matrix.t[0]=0; matrix.t[1]=0; matrix.t[2]=0;
            do {
                RoomM273Drop *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                short *wave;
                if(!particle) break;
                wave=(short *)&((int *)rcossin_tbl)[(i*4096/6)&4095];
                particle->ring.x=angle+wave[1]*160/4096;
                particle->ring.y=wave[0]*width/4096+(*heading+2048);
                particle->ring.z=0; particle->ring.pad=1;
                RotMatrixYXZ(&particle->ring,transform);
                gte_ldrotmatrix(transform);
                gte_ldtransmatrix(transform);
                { Vector *input=&g_RoomM273SweepDropVector; gte_ldv0(input); }
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_mvmva_rotation_v0_translation_sf12();
                gte_stsv((GteShortVector *)&particle->vx);
                particle->position.x=heading[-50];
                particle->position.y=heading[-49];
                particle->position.z=heading[-48];
                particle->position.pad=0; particle->touched=0;
                i++;
            } while(i<6);
            /* Failure in the six-particle ring does not skip this ring. */
            i=0; width=spread/2; heading=&g_RoomM273SweepState.target;
            do {
                RoomM273Drop *particle=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                short *wave;
                if(!particle) break;
                {
                    register int *table asm("$3")=(int *)rcossin_tbl;
                    wave=(short *)&table[((unsigned int)i<<10)&3072];
                }
                particle->ring.x=angle+wave[1]*96/4096;
                particle->ring.y=wave[0]*width/4096+(*heading+2048);
                particle->ring.z=0; particle->ring.pad=1;
                asm("" : : "r"(&particle->ring));
                transform=&matrix;
                RotMatrixYXZ(&particle->ring,transform);
                gte_ldrotmatrix(transform);
                gte_ldtransmatrix(transform);
                { Vector *input=&g_RoomM273SweepDropVector; gte_ldv0(input); }
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_mvmva_rotation_v0_translation_sf12();
                gte_stsv((GteShortVector *)&particle->vx);
                particle->position.x=heading[-50];
                particle->position.y=heading[-49];
                particle->position.z=heading[-48];
                particle->position.pad=0; particle->touched=0;
                i++;
            } while(i<4);
            Asset_Find08w(0x5D1,ANIM_OWNER_AS(RoomM273EffectStateContext)->state.directed_rings->owner->value,g_RoomM273SweepPoint.x,g_RoomM273SweepPoint.y,g_RoomM273SweepPoint.z);
        }
        break;
    }
    case 2: {
        unsigned int index=g_FieldFxTpageSlot2;
        unsigned short palette;
        g_FieldFxSpriteParams.parameter00=16; g_FieldFxSpriteParams.parameter02=1;
        g_FieldFxSpriteParams.extent_x=16; g_FieldFxSpriteParams.extent_y=16;
        g_FieldFxSpriteParams.extent_x=16; g_FieldFxSpriteParams.extent_y=16;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=2; g_FieldFxSpriteParams.parameter06=0; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

/* The batch emitter fills the eight-byte records rendered by this callback. */

int RoomM273_SweepSplashCallback(int mode, GteShortVector *position) {
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
        position->y -= 6;
    } else if (mode == 2) {
        int frame = g_FieldFxCurrentAge - 1;
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        int clut;
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(64, palette);
        FieldFx_DrawShapeQuads(position, 0, 4096, 4096,
            44 + (s16)g_FieldFxSpriteParams.parameter02 * g_RoomM273SweepSplashScale[frame / 4], clut, 1,
            (short)rcossin_tbl[(((unsigned int)frame << 9) & 0x3E00) / 4].word >> 5, 0);
    }
    return 0;
}

int RoomM273_SweepSplashEmitter(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int stack_pad[2];
    switch(mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,40,
                             (FieldFxCallbackListCallback)RoomM273_SweepSplashCallback);
    case 1: {
        int i;
        unsigned short *x,*y;
        if(g_RoomM273SweepSplashBatch.stopped) return 2;
        i=0;
        if(g_RoomM273SweepSplashBatch.count>0) {
            RoomM273RisingBatchBuffer *batch=&g_RoomM273SweepSplashBatch;
            y=batch->y;
            x=batch->x;
            do {
                int offset;
                GteShortVector *out=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if(!out) break;
                out->x=*x;
                out->y=*y++-128;
                asm("" : "=r"(i) : "0"(i), "r"(x));
                ++x;
                offset=i*2;
                out->z=(&((unsigned short *)&((char *)batch)[offset])[24])[0];
                out->pad=0;
                ++i;
            } while(i<batch->count);
        }
        g_RoomM273SweepSplashBatch.count=0;
        break;
    }
    case 2: {
        unsigned int index=g_FieldFxTpageSlot3;
        unsigned short palette;
        g_FieldFxSpriteParams.parameter00=32; g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        g_FieldFxSpriteParams.extent_x=32; g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=3; g_FieldFxSpriteParams.parameter06=0; g_FieldFxSpriteParams.parameter0A=0; g_FieldFxSpriteParams.depth=0;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

#undef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#undef ROOMLIB_POLL_RESET_POOL_EXPR
#undef ROOMLIB_POLL_RESET_FUNC
#undef ROOMLIB_POLL_RESET_CALLBACK
#undef ROOMLIB_POLL_RESET_FLAG
#undef ROOMLIB_POLL_RESET_COUNTER
#undef ROOMLIB_POLL_RESET_SEED

/* Sprite callback and poll/reset controller share one pool and layout. */

int RoomM273_SweepDropHitCallback(int mode, Vector *position) {
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 8) return 1;
    } else if (mode == 2) {
        int frame = g_FieldFxCurrentAge - 1;
        int kind, palette;
        int size;
        int sample;
        unsigned short clut;
        sample = rcossin_tbl[(((unsigned int)frame << 9) & 0x3E00) / 4].word;
        kind = g_FieldFxSpriteParams.palette;
        size = sample + 2048;
        palette = g_FieldFxPaletteTable[kind];
        clut = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 9 : palette + 5);
        FieldFx_DrawShapeQuads(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)rcossin_tbl[(((unsigned int)frame << 10) & 0x3C00) / 4].word >> 5, 0);
    }
    return 0;
}

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR ANIM_POOL_AS(RoomM273PoolContext)->pool
#define ROOMLIB_POLL_RESET_FUNC RoomM273_SweepDropHitEmitter
#define ROOMLIB_POLL_RESET_CALLBACK RoomM273_SweepDropHitCallback
#define ROOMLIB_POLL_RESET_FLAG g_RoomM273SweepState.done
#define ROOMLIB_POLL_RESET_COUNTER g_RoomM273SweepState.hit.vector.pad
#define ROOMLIB_POLL_RESET_SEED g_RoomM273SweepState.hit
#include "../room_lib/poll_reset_actor.inc"

/* The timed emitter fills the eight-byte records rendered by this callback. */
int RoomM273_SweepRisingCallback(int mode, Particle *input) {
    Particle *particle = input;
    /* Layout only: original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 32) return 1;
        else {
            unsigned short y = ((volatile Particle *)particle)->y;
            s16 speed = ((volatile Particle *)particle)->speed;
            int delta = ((Particle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = g_FieldFxCurrentAge - 1;
        int kind = g_FieldFxSpriteParams.palette;
        int sample, palette, size, shade;
        int paletteOffset, sizeOffset;
        unsigned int sampleOffset;
        int clut;
        sampleOffset = ((unsigned int)frame << 8) & 0x3F00;
        asm("" : : "r"(sampleOffset), "r"(frame));
        paletteOffset = kind * 2;
        sample = rcossin_tbl[sampleOffset / 4].word;
        asm("" : : "r"(sample), "r"(paletteOffset));
        sizeOffset = ((unsigned int)frame << 7) & 0x3F80;
        palette = *(unsigned short *)&((char *)g_FieldFxPaletteTable)[paletteOffset];
        shade = (short)sample >> 5;
        size = *(short *)&((char *)rcossin_tbl)[sizeOffset]
            + 8192;
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(16, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)particle, 0, size, size, (s16)g_FieldFxSpriteParams.parameter02 * (frame / 4) + 200,
            clut, 1, shade, (struct RenderColor *)g_RoomM273SweepRisingColor);
    }
    return 0;
}

int RoomM273_SweepRisingEmitter(int mode) {
    switch(mode) {
    case 0:
        g_RoomM273SweepRisingDelay=0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,12,
                             (FieldFxCallbackListCallback)RoomM273_SweepRisingCallback);
    case 1: {
        Particle *output;
        if(g_RoomM273SweepState.done) return 2;
        if(!g_RoomM273SweepState.in_window) return 0;
        if(g_RoomM273SweepRisingDelay-- > 0) return 0;
        output=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!output) return 0;
        ((volatile Particle *)output)->x=g_RoomM273SweepPoint.x;
        ((volatile Particle *)output)->y=g_RoomM273SweepPoint.y-256;
        output->z=g_RoomM273SweepPoint.z;
        g_RoomM273SweepRisingDelay=2;
        ((volatile Particle *)output)->speed=0;
        break;
    }
    case 2: {
        unsigned int index=g_FieldFxTpageSlot2;
        unsigned short palette;
        g_FieldFxSpriteParams.parameter00=16;
        g_FieldFxSpriteParams.parameter02=1;
        g_FieldFxSpriteParams.extent_x=16;
        g_FieldFxSpriteParams.extent_y=16;
        g_FieldFxSpriteParams.extent_x=16;
        g_FieldFxSpriteParams.extent_y=16;
        palette=g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette=2;
        g_FieldFxSpriteParams.parameter06=0;
        g_FieldFxSpriteParams.parameter0A=0;
        g_FieldFxSpriteParams.depth=0;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

/* The position emitter fills the eight-byte records rendered by this callback. */
int RoomM273_SweepFlashCallback(int mode,Vector *input) {
    Vector rotation,position;
    if(mode==1) {
        if(g_FieldFxCurrentAge>=16) return 1;
    } else if(mode==2) {
        int counter=g_FieldFxCurrentAge;
        int frame=counter-1;
        int kind=g_FieldFxSpriteParams.palette;
        int firstSample=rcossin_tbl[((unsigned int)(u8)frame<<6)&0xFC0].word;
        int shade,size,palette;
        int secondSample;
        unsigned short clut;
        rotation.x=0; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=0;
        position=*input;
        palette=g_FieldFxPaletteTable[kind];
        shade=firstSample>>21;
        size=(short)firstSample*3+2048;
        if(kind==4 && g_FieldFxAltPaletteFlag) palette+=4;
        clut=GetClut(80,palette);
        FieldFx_DrawShapeQuads(&position,(GteRotation *)&rotation,size,size,74,clut,1,shade,0);
        rotation.z>>=1;
        secondSample=rcossin_tbl[((unsigned int)(u8)frame<<6)&0xFC0].word;
        shade=secondSample>>21;
        size=(short)secondSample+1024;
        FieldFx_DrawStarFan(&position,512,512,16,(GteRotation *)&rotation,size,size,(struct RenderColor *)g_RoomM273BlackColor,(struct RenderColor *)g_RoomM273SweepFlashColorA,shade,1);
        rotation.x=1024; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=1;
        position.y=g_RoomFloorY->raw;
        FieldFx_DrawStarFan(&position,768,768,8,(GteRotation *)&rotation,4096,4096,(struct RenderColor *)g_RoomM273SweepFlashColorB,(struct RenderColor *)g_RoomM273BlackColor,shade,1);
    }
    return 0;
}

int RoomM273_SweepFlashEmitter(int mode) {
    switch(mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,4,
                             (FieldFxCallbackListCallback)RoomM273_SweepFlashCallback);
    case 1: {
        Vector *output;
        if(g_RoomM273SweepState.done) return 2;
        if(!g_RoomM273SweepState.in_contact) return 0;
        output=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!output) return 0;
        ((volatile Vector *)output)->x=g_RoomM273SweepPoint.x;
        ((volatile Vector *)output)->y=g_RoomM273SweepPoint.y;
        output->z=g_RoomM273SweepPoint.z;
        break;
    }
    case 2: {
        unsigned int index=g_FieldFxTpageSlot3;
        unsigned short palette;
        g_FieldFxSpriteParams.parameter00=32;
        g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32;
        g_FieldFxSpriteParams.extent_y=32;
        g_FieldFxSpriteParams.extent_x=32;
        g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];

        g_FieldFxSpriteParams.palette=3;
        g_FieldFxSpriteParams.parameter06=0;
        g_FieldFxSpriteParams.parameter0A=0;
        g_FieldFxSpriteParams.depth=64;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

#undef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#undef ROOMLIB_POLL_RESET_POOL_EXPR
#undef ROOMLIB_POLL_RESET_FUNC
#undef ROOMLIB_POLL_RESET_CALLBACK
#undef ROOMLIB_POLL_RESET_FLAG
#undef ROOMLIB_POLL_RESET_COUNTER
#undef ROOMLIB_POLL_RESET_SEED

/* Animation 13 controller: turns the boss toward the player on entry, loops
 * the sweep window while loops remain, publishes the sweep attack angle and
 * transforms the sweep point. */
int RoomM273_SweepController(int mode) {
    RoomM273BossInstance *instance = ANIM_OWNER_AS(RoomM273BossActor)->instance;
    RoomM273SweepStep *step;
    GteShortVector offset;
    s16 frame;
    s16 angle;
    s16 value;

    if (mode == 0) {
        g_RoomM273SweepState.started = 0;
        g_RoomM273SweepState.done = 0;
        g_RoomM273SweepState.in_window = 0;
        g_RoomM273SweepState.in_contact = 0;
        g_RoomM273SweepState.step = 0;
        g_RoomM273SweepState.count = 0;
        g_RoomM273SweepState.hit.vector.pad = 0;
        g_RoomM273SweepState.floor = g_RoomFloorY->raw - 0x180;
        instance->owner->flags |= 0x40000000;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        if (instance->animation == 13) {
            frame = instance->time.bits.frame;
            step = &g_RoomM273SweepSteps[g_RoomM273SweepState.side];
            if (frame == 0) {
                instance->time.fixed = step->start << 16;
                angle = (ratan2(instance->x - g_PlayerEntity->pos_x,
                                       instance->z - g_PlayerEntity->pos_z)
                         - instance->yaw) & 0xFFF;
                if (g_RoomM273SweepState.side != 0) {
                    if (angle <= 0x800) g_RoomM273SweepState.turn = angle >> 6;
                    else g_RoomM273SweepState.turn = 0;
                } else {
                    angle = 0x1000 - angle;
                    if (angle <= 0x800) g_RoomM273SweepState.turn = -angle >> 6;
                    else g_RoomM273SweepState.turn = 0;
                }
            } else {
                if (frame >= step->end) {
                    g_RoomM273SweepState.done = 1;
                    if (instance->owner) *instance->owner->status = 4;
                    return 1;
                }
                if (frame >= step->loop_end && g_RoomM273SweepState.loops > 0) {
                    instance->time.fixed = step->loop_start << 16;
                    g_RoomM273SweepState.loops--;
                }
            }
            if (frame >= step->loop_start) g_RoomM273SweepState.started = 1;
            g_RoomM273SweepState.in_window = frame >= step->loop_start && frame <= step->loop_end;
            {
                RoomM273BossInstance *current = ANIM_OWNER_AS(RoomM273BossActor)->instance;
                s16 previous;
                frame = current->time.bits.frame;
                previous = current->previous.bits.frame;
                g_RoomM273SweepState.in_contact = frame >= g_RoomM273SweepState.contact && previous < g_RoomM273SweepState.contact;
            }
            if (frame >= step->loop_start && frame <= step->loop_end) {
                value = step->scale * g_RoomM273SweepState.step / g_RoomM273SweepState.divisor;
                instance->attack_kind = 0x15;
                instance->attack_power = 2;
                instance->attack_angle = value;
                g_RoomM273SweepState.target = step->offset + (value + instance->yaw);
                g_RoomM273SweepState.step++;
            } else {
                instance->attack_kind = 0;
                instance->attack_power = 0;
            }
            if (frame > step->loop_end) g_RoomM273SweepState.turn >>= 1;
            instance->yaw += g_RoomM273SweepState.turn;
        }
    }
    offset.x = 0;
    offset.y = 0;
    offset.z = -0x180;
    {
        GteMatrix *matrix = instance->transforms + 22;
        GteShortVector *out;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        out = &g_RoomM273SweepPoint;
        gte_stsv(out);
    }
    return 0;
}

int RoomM273_SwayShardTrailCallback(int mode, GteShortVector *position) {
    int frame, sample, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 9) return 1;
    } else if (mode == 2) {
        frame = g_FieldFxCurrentAge;
        if (frame - 2 < 0) return 0;

        sample = rcossin_tbl[((frame - 1) << 7) & 0xF80].part.cos;
        size = sample * 2;
        kind = g_FieldFxSpriteParams.palette;
        /* Keep the table read before the palette lookup. */
        asm volatile("" ::: "memory");
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;

        clut = GetClut(32, palette);
        FieldFx_DrawShapeQuads(position, 0, (s16)size, (s16)size,
                       (s16)g_FieldFxSpriteParams.parameter02 + 216, clut, 1,
                       /* The upper half of each packed trig entry is signed. */
                       (s16)(rcossin_tbl[((g_FieldFxCurrentAge - 1) << 7) & 0xF80].word >> 16) >> 5,
                       (RenderColor *)g_RoomM273SwayShardTrailColor);
    }
    return 0;
}

/* Sway shard: flies until it hits the player or the floor (recording the
 * impact in the sway point table), leaving a trail; draws a glow, two
 * crossed flares and two floor rings. */
int RoomM273_SwayShardCallback(int mode, RoomM273SwayShard *shard) {
    GteShortVector position;
    GteRotation spin;

    if (mode == 1) {
        if (shard->state == 0 && g_RoomM273SwayState.cooldown == 0 &&
            shard->position.y > (s16)g_RoomFloorY->raw - 0x240) {
            s16 dx = g_PlayerEntity->render_object.model_matrix.translation[0] - shard->position.x;
            s16 dz = g_PlayerEntity->render_object.model_matrix.translation[2] - shard->position.z;
            if ((s16)Math_IntSqrt(dx * dx + dz * dz) < 0x80) {
                shard->state = 1;
                g_PlayerEntity->state->flags |= 0x4000;
                if (ANIM_OWNER_AS(RoomM273BossActor)->instance->owner)
                    ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->flags |= 0x80000000;
                g_RoomM273SwayState.hits[shard->side].x = shard->position.x;
                g_RoomM273SwayState.hits[shard->side].y = shard->position.y;
                g_RoomM273SwayState.hits[shard->side].z = shard->position.z;
                g_RoomM273SwayState.hits[shard->side].pad = 2;
                Asset_Find08w(0x5FC, ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->sound, shard->position.x,
                              shard->position.y, shard->position.z);
                g_RoomM273SwayState.cooldown = 0x28;
                return 1;
            }
        }
        shard->position.x += shard->velocity.x;
        shard->position.y += shard->velocity.y;
        shard->position.z += shard->velocity.z;
        if (shard->position.y >= (s16)g_RoomFloorY->raw) {
            g_RoomM273SwayState.hits[shard->side].x = shard->position.x;
            g_RoomM273SwayState.hits[shard->side].y = shard->position.y;
            g_RoomM273SwayState.hits[shard->side].z = shard->position.z;
            g_RoomM273SwayState.hits[shard->side].pad = 3;
            Asset_Find08w(0x5FC, ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->sound, shard->position.x,
                          shard->position.y, shard->position.z);
            return 1;
        } else {
            s16 frame = g_FieldFxCurrentAge & 7;
            GteShortVector *trail;
            if (frame < 4) shard->frame = frame;
            else shard->frame = frame + 12;
            trail = FieldFx_AcquireCallbackSlot(g_RoomM273SwayShardTrailPool);
            if (trail == 0) return 0;
            trail->x = shard->position.x;
            trail->y = shard->position.y;
            trail->z = shard->position.z;
        }
    } else if (mode == 2) {
        int i;
        int kind;
        int palette;
        position.x = shard->position.x;
        position.y = shard->position.y;
        position.z = shard->position.z;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        FieldFx_DrawShapeQuads(&position, 0, 0xA00, 0xA00,
                      (s16)g_FieldFxSpriteParams.parameter02 * shard->frame,
                      GetClut(0x10, palette), 1, 0x80, 0);
        spin.x = g_FieldFxCurrentAge * 0x300;
        spin.y = ANIM_OWNER_AS(RoomM273BossActor)->instance->yaw;
        spin.z = 0;
        spin.flags = 1;
        for (i = 0; i < 2; i++) {
            FieldFx_DrawRingBand(&position, 0x60, 0x88, 8, &spin, 0x1000, 0x1000, &g_RoomM273SwayShardRingColor,
                          (RenderColor *)g_RoomM273BlackColor, 0x80, 1);
            spin.y = spin.x;
            spin.x = 0;
        }
        spin.x = 0;
        spin.y = 0;
        spin.flags = 0;
        spin.z = g_FieldFxCurrentAge << 8;
        FieldFx_DrawStarFan(&position, 0x100, 0x100, 8, &spin, 0x1000, 0x1000, &g_RoomM273SwayShardStarColor,
                      (RenderColor *)g_RoomM273BlackColor, 0x80, 1);
        position.y = g_RoomFloorY->raw;
        FieldFx_DrawStarFan(&position, 0xC0, 0xC0, 8, 0, 0x1000, 0x1000, &g_RoomM273SwayShardInnerStarColor,
                      (RenderColor *)g_RoomM273BlackColor, 0x80, 1);
    }
    return 0;
}

/* Spawns a shard from each sway point aimed at the player while animation
 * 15 swings through, and draws the shard and glow pools. */
int RoomM273_SwayShardEmitter(int mode) {
    GteShortVector target;
    GteShortVector angles;
    GteMatrix matrix;
    int count;

    switch (mode) {
    case 0:
        count = FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 0x14, 4,
                              (FieldFxCallbackListCallback)RoomM273_SwayShardCallback);
        count += FieldFx_AllocCallbackList(&g_RoomM273SwayShardTrailPool, count, 8, 0x14, RoomM273_SwayShardTrailCallback);
        return count;
    case 1:
        if (g_RoomM273SwayState.done) return 2;
        if (ANIM_OWNER_AS(RoomM273BossActor)->instance->animation == 15 && g_RoomM273SwayState.frame >= 4 &&
            g_RoomM273SwayState.frame_1A < 4) {
            int i;
            for (i = 0; i < 2; i++) {
                RoomM273SwayShard *shard = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                RoomM273BossPlayer *player;
                int dx;
                int dy;
                int dz;
                if (shard == 0) break;
                shard->side = i;
                shard->state = 0;
                player = (RoomM273BossPlayer *)g_PlayerEntity;
                shard->position.x = g_RoomM273SwayState.points[i].x;
                shard->position.y = g_RoomM273SwayState.points[i].y;
                shard->position.z = g_RoomM273SwayState.points[i].z;
                target.x = player->location[0].parts.integer;
                target.y = player->location[1].parts.integer;
                target.z = player->location[2].parts.integer;
                dx = target.x - shard->position.x;
                dz = target.z - shard->position.z;
                dy = target.y - shard->position.y;
                angles.x = (ratan2(dy, Math_IntSqrt(dx * dx + dz * dz)) & 0xFFF) - 0x30;
                angles.y = ANIM_OWNER_AS(RoomM273BossActor)->instance->yaw;
                angles.z = 0;
                if (angles.x < 0x100) angles.x = 0x100;
                if (angles.x > 0x280) angles.x = 0x280;
                RotMatrixYXZ(&angles, &matrix);
                ApplyMatrixSV(&matrix, &g_RoomM273SwayShardVelocity, &shard->velocity);
            }
            target.x = (g_RoomM273SwayState.points[0].x + g_RoomM273SwayState.points[1].x) / 2;
            target.y = (g_RoomM273SwayState.points[0].y + g_RoomM273SwayState.points[1].y) / 2;
            target.z = (g_RoomM273SwayState.points[0].z + g_RoomM273SwayState.points[1].z) / 2;
            Asset_Find08w(0x5D2, ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->sound, target.x, target.y,
                          target.z);
        }
        FieldFx_UpdateCallbackList(g_RoomM273SwayShardTrailPool);
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_DrawCallbackList(g_RoomM273SwayShardTrailPool);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Paired ground sprites and the boss ground ring share sway points and pool records. */

/* The emitter owns the eight-byte pool whose entries this callback renders. */

int RoomM273_SwayGroundCallback(int mode,Vector *input) {
    Vector position;
    if(mode==1) {
        if(g_FieldFxCurrentAge>=32) return 1;
    } else if(mode==2) {
        unsigned int phase=(unsigned int)(g_FieldFxCurrentAge-1)<<5;
        int shadeSample=*(short *)&((char *)g_RcossinTblCos)[((phase
            +1024)&4095)*4];
        int sizeSample=*(short *)&((char *)g_RcossinTblCos)[(phase&4095)*4];
        unsigned int shade;
        int size,kind,palette;
        unsigned short clut;
        position=*input;
        kind=g_FieldFxSpriteParams.palette;
        shade=(unsigned int)(shadeSample+4096)>>5;
        size=sizeSample*2+4096;
        palette=g_FieldFxPaletteTable[kind];
        clut=GetClut(0,(kind==4 && g_FieldFxAltPaletteFlag) ? palette+8 : palette+4);
        FieldFx_DrawShapeQuads(&position,0,(short)size,(short)size,100,clut,1,(short)shade,0);
        position.y=g_RoomFloorY->raw;
        FieldFx_DrawStarFan(&position,384,384,8,&g_RoomM273FloorRotation,4096,4096,
            (RenderColor *)g_RoomM273SwayGroundColor,(RenderColor *)g_RoomM273BlackColor,(short)shade,1);
    }
    return 0;
}

int RoomM273_SwayGroundEmitter(int mode) {
    switch(mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,4,
                             (FieldFxCallbackListCallback)RoomM273_SwayGroundCallback);
    case 1: {
        Record *source;
        int i;
        if(g_RoomM273SwayState.done) return 2;
        if(g_RoomM273SwayState.animation!=15) return 0;
        if(g_RoomM273SwayState.frame<4) return 0;
        if(g_RoomM273SwayState.frame_1A>=4) break;
        i=0;
        source=(Record *)g_RoomM273SwayState.points;
        for(;i<2;++i) {
            Record *output=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if(!output) break;
            *output=*source;
            ++source;
        }
        break;
    }
    case 2: {
        unsigned int index=g_FieldFxTpageSlot11;
        unsigned short palette;
        g_FieldFxSpriteParams.parameter00=32;
        g_FieldFxSpriteParams.parameter02=2;
        g_FieldFxSpriteParams.extent_x=32;
        g_FieldFxSpriteParams.extent_y=32;
        g_FieldFxSpriteParams.extent_x=32;
        g_FieldFxSpriteParams.extent_y=32;
        palette=g_FieldFxTpageTable[index];

        g_FieldFxSpriteParams.palette=3;
        g_FieldFxSpriteParams.parameter06=1;
        g_FieldFxSpriteParams.parameter0A=0;
        g_FieldFxSpriteParams.depth=32;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

/* Ground ring: while it opens (frames up to 24) it knocks the player back
 * once when the player stands inside the pulsing radius; while drawing it
 * shows the swelling ring model, a short flash and two rotating rings. */
int RoomM273_SwayGroundRingCallback(int mode, GteShortVector *position) {
    GteRotation rotation;
    GteMatrix matrix;
    GteVector scale;
    s16 unused[4];
    int frame;
    int size;
    int fade;
    int value;
    int i;

    if (mode == 1) {
        int now = g_FieldFxCurrentAge;
        if (now >= 0x28) return 1;
        if (now >= 0x19) return 0;
        if (position->pad != 0) return 0;
        if (g_RoomM273SwayState.cooldown != 0) return 0;
        {
            int radius = rcossin_tbl[((now << 11) / 40) & 0xFFF].bits.sin / 16 + 0x80;
            int dx = g_PlayerEntity->render_object.model_matrix.translation[0] - position->x;
            int dz = g_PlayerEntity->render_object.model_matrix.translation[2] - position->z;
            if (radius < Math_IntSqrt(dx * dx + dz * dz)) return 0;
        }
        position->pad = 1;
        g_PlayerEntity->state->flags |= 0x4000;
        if (ANIM_OWNER_AS(RoomM273BossActor)->instance->owner) {
            ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->flags |= 0x80000000;
            g_RoomM273SwayState.cooldown = 0x28;
        }
        return 0;
    }
    if (mode != 2) return 0;
    frame = g_FieldFxCurrentAge - 1;
    if (g_FieldFxCurrentAge >= 5 && g_FieldFxCurrentAge < 0x25) {
        int page;
        int kind;
        int palette;
        int cosine;
        int sine;
        i = g_FieldFxCurrentAge - 5;
        if (i < 8) value = rcossin_tbl[(i << 7) & 0xF80].bits.sin;
        else value = rcossin_tbl[(((g_FieldFxCurrentAge - 13) << 10) / 24) & 0xFFF].bits.cos;
        page = (g_FieldFxTpageTable[g_FieldFxTpageSlot11] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
        fade = value >> 6;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        size = value >> 2;
        FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6 : palette + 2));
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(g_RoomM273SweepState.model);
        FieldFx_ModelScaleColors(g_RoomM273SweepState.model, (u16)fade);
        cosine = rcos(frame << 8);
        sine = rsin(frame << 8);
        matrix.m[0][2] = sine;
        matrix.m[2][0] = -sine;
        matrix.m[0][0] = cosine;
        matrix.m[2][2] = cosine;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        memset(&scale, 0, sizeof(scale));
        scale.x = size;
        scale.y = size;
        scale.z = size;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position->x;
        matrix.t[1] = position->y;
        matrix.t[2] = position->z;
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM273SweepState.model, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM273SweepState.model);
    }
    g_FieldFxSpriteParams.parameter00 = 0x20;
    g_FieldFxSpriteParams.parameter02 = 2;
    g_FieldFxSpriteParams.extent_x = 0x20;
    g_FieldFxSpriteParams.extent_y = 0x20;
    g_FieldFxSpriteParams.extent_x = 0x20;
    g_FieldFxSpriteParams.extent_y = 0x20;
    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot3)];
    g_FieldFxSpriteParams.palette = 3;
    g_FieldFxSpriteParams.parameter06 = 0;
    if (frame < 8) {
        int kind = g_FieldFxSpriteParams.palette;
        int palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        size = rcossin_tbl[(frame << 7) & 0xF80].bits.sin + 0x1800;
        FieldFx_DrawShapeQuads(position, 0, size, size, (s16)g_FieldFxSpriteParams.parameter02 * frame + 0x80,
                      GetClut(0x10, palette), 1, 0x80, 0);
    }
    rotation.x = 0x400;
    rotation.y = -frame << 9;
    rotation.flags = 1;
    rotation.z = 0;
    fade = rcossin_tbl[(frame * 25) & 0xFFF].bits.cos >> 5;
    value = rcossin_tbl[(frame * 51) & 0xFFF].bits.sin / 16;
    for (i = 0; i < 2; i++) {
        FieldFx_DrawStarFan(position, value, value, 8, &rotation, 0x1000, 0x1000,
                      &g_RoomM273SwayGroundRingColors[0], &g_RoomM273SwayGroundRingColors[1], fade, 1);
        value += 0x100;
    }
    return 0;
}

int RoomM273_SwayGroundRingEmitter(int mode) {
    switch (mode) {
    case 0:
        g_RoomM273SweepState.model = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5541704);
        FieldFx_ModelScrollUv(g_RoomM273SweepState.model, 0, 0);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 4,
                             (FieldFxCallbackListCallback)RoomM273_SwayGroundRingCallback);
    case 1: {
        int i;
        unsigned char *flags;
        Entry *source;
        unsigned char *anchor = (unsigned char *)&g_RoomM273SwayHitView + 30;
        if (*anchor) return 2;
        i = 0;
        flags = anchor - 24;
        source = (Entry *)(anchor - 30);
        for (; i < 2; ++i) {
            asm("" : "=r"(source) : "0"(source));
            if (*(unsigned short *)flags & 1) {
                Entry *entry = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if (!entry) return 0;
                entry->emitter.x = source->emitter.x;
                entry->emitter.y = source->emitter.y;
                entry->emitter.z = source->emitter.z;
                entry->emitter.flags = 0;
                *(unsigned short *)flags &= 0xFFFE;
            }
            flags += 8;
            ++source;
        }
        break;
    }
    case 2:
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        break;
    }
    return 0;
}

int RoomM273_SwayBurstCallback(int mode, RoomM273RisingParticle *particle) {
    if (mode == 1) {
        int frame = g_FieldFxCurrentAge;
        if (frame >= 36) return 1;
        if (frame >= 4) {
            unsigned short y = ((volatile RoomM273RisingParticle *)particle)->y;
            s16 speed = ((volatile RoomM273RisingParticle *)particle)->speed;
            int delta = (particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = g_FieldFxCurrentAge - 5;
        int kind, palette, clut;
        if (frame < 0) return 0;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(16, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)particle, 0, 12288, 12288,
            (s16)g_FieldFxSpriteParams.parameter02 * (frame / 4) + 200, clut, 1,
            (short)((int *)rcossin_tbl)[(((unsigned int)frame << 8) & 0x3F00) / 4] >> 5, 0);
    }
    return 0;
}

int RoomM273_SwayBurstEmitter(int mode)
{
    RoomM273TemplatePoint *template;
    RoomM273DelayedBurstRecord *particle;
    RoomM273TrigEntry *entry;
    int i;
    int j;
    int angle;
    int offset;
    int flagOffset;
    int sixteen;
    u16 paletteIndex;
    u8 *stop;
    u16 *flag;
    RoomM273TemplatePoint *source;
    u16 palette;

    switch (mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 24, RoomM273_SwayBurstCallback);
    case 1:
        stop = &g_RoomM273SwayState.done;
        if (*stop) return 2;
        i = 0;
        template = (RoomM273TemplatePoint *)(stop - 0x1E);
        offset = 0;
        do {
            flag = (u16 *)(g_RoomM273SwayHitFlags + offset);
            if (*flag & 2) {
                j = 0;
                flagOffset = offset;
                source = template;
                do {
                    particle = (RoomM273DelayedBurstRecord *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    if (!particle) break;
                    particle->emitter = *source;
                    angle = ((j << 12) / 6) & 0xFFF;
                    entry = &rcossin_tbl[angle];
                    particle->emitter.x += (entry->low * 3 * 64) / 4096;
                    particle->emitter.z += (entry->high * 3 * 64) / 4096;
                    particle->emitter.flags = 0;
                    *(u16 *)(g_RoomM273SwayHitFlags + flagOffset) &= ~2;
                    j++;
                } while (j < 6);
            }
            template++;
            i++;
            offset += 8;
        } while (i < 2);
        break;
    case 2:
        paletteIndex = g_FieldFxTpageSlot2;
        sixteen = 16;
        g_FieldFxSpriteParams.parameter00 = sixteen;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = sixteen;
        g_FieldFxSpriteParams.extent_y = sixteen;
        g_FieldFxSpriteParams.extent_x = sixteen;
        g_FieldFxSpriteParams.extent_y = sixteen;
        palette = g_FieldFxTpageTable[paletteIndex];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
    return 0;
}

/* Pointer sprite pool and boss sway state share g_RoomM273SwayState points. */

/* The emitter owns the pointer entries rendered by this callback. */

int RoomM273_SwayPointerCallback(int mode, RoomM273PointerPoolEffect *effect) {
    /* Layout only: the original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (mode == 2) {
        int frame = (short)(g_FieldFxCurrentAge - 1);
        int kind = g_FieldFxSpriteParams.palette;
        unsigned int sizeOffset, shadeOffset;
        s16 size;
        s16 shade;
        int palette;
        unsigned short clut;
        sizeOffset = ((unsigned int)frame << 8) & 0x3F00;
        shadeOffset = ((unsigned int)frame << 9) & 0x3E00;
        size = rcossin_tbl[sizeOffset / 4].word + 2048;
        shade = (short)rcossin_tbl[shadeOffset / 4].word >> 6;

        palette = g_FieldFxPaletteTable[kind];
        clut = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 7 : palette + 3);
        FieldFx_DrawShapeQuads(effect->position, 0, size, size,
            64, clut, 1, shade, 0);
    }
    return 0;
}

int RoomM273_SwayPointerEmitter(int mode) {
    switch(mode) {
    case 0:
        g_RoomM273SwayPointerSpawned=0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool,4,6,
                             (FieldFxCallbackListCallback)RoomM273_SwayPointerCallback);
    case 1: {
        int i;
        Position *position;
        if(g_RoomM273SwayPointerSpawned) return 2;
        if(g_RoomM273SwayState.animation==15 && g_RoomM273SwayState.frame>=25) {
            g_RoomM273SwayPointerSpawned=1;
            return 2;
        }
        if(g_FieldFxCurrentAge&7) return 0;
        i=0;
        position=g_RoomM273SwayState.points;
        for(;i<2;++i) {
            RoomM273PointerPoolEffect *output=FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if(!output) break;
            output->position=position;
            ++position;
        }
        break;
    }
    case 2: {
        unsigned int index=g_FieldFxTpageSlot11;
        unsigned short palette;
        g_FieldFxSpriteParams.parameter00=64;
        g_FieldFxSpriteParams.parameter02=4;
        g_FieldFxSpriteParams.extent_x=64;
        g_FieldFxSpriteParams.extent_y=64;
        g_FieldFxSpriteParams.extent_x=64;
        g_FieldFxSpriteParams.extent_y=64;
        palette=g_FieldFxTpageTable[index];

        g_FieldFxSpriteParams.palette=3;
        g_FieldFxSpriteParams.parameter06=1;
        g_FieldFxSpriteParams.parameter0A=0;
        g_FieldFxSpriteParams.depth=32;
        g_FieldFxSpriteParams.tpage=palette;
        break;
    }
    }
    return 0;
}

/* Animation 15 controller: transforms two model points, then sways the
 * boss toward or away from the player for eight frames. */
int RoomM273_SwayController(int mode) {
    RoomM273BossInstance *instance = ANIM_OWNER_AS(RoomM273BossActor)->instance;
    GteShortVector offset;
    int value; /* loop index, then the player heading */
    int frame;
    int frame_1A;
    GteShortVector *point;

    if (mode == 0) {
        g_RoomM273SwayState.done = 0;
        instance->owner->flags |= 0x40000000;
        g_RoomM273SwayState.points[0].pad = 0;
        g_RoomM273SwayState.points[1].pad = 1;
        g_RoomM273SwayState.hits[0].pad = 0;
        g_RoomM273SwayState.hits[1].pad = 0;
        g_RoomM273SwayState.sway_timer = 0;
        g_RoomM273SwayState.cooldown = 0;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        frame = instance->time.parts.frame;
        frame_1A = instance->previous.parts.frame;
        g_RoomM273SwayState.animation = instance->animation;
        g_RoomM273SwayState.frame = frame;
        g_RoomM273SwayState.frame_1A = frame_1A;
        offset.x = 0;
        offset.y = 0;
        offset.z = -0x130;
        point = g_RoomM273SwayState.points;
        for (value = 0; value < 2; value++) {
            GteMatrix *m = instance->transforms;
            if (value) m += 36;
            else m += 25;
            gte_ldrotmatrix(m);
            gte_ldtransmatrix(m);
            gte_ldv0(&offset);
            gte_rtv0tr_mac();
            gte_stsv(&point[value]);
        }
        if (instance->animation == 15) {
            if ((s16)frame >= instance->frame_count - 1) {
                g_RoomM273SwayState.done = 1;
                if (instance->owner) *instance->owner->status = 4;
                return 1;
            }
            if ((s16)frame >= 0x16) {
                if (g_RoomM273SwayState.repeat-- > 0) instance->time.fixed = 0;
            }
        }
        if (g_RoomM273SwayState.repeat && (s16)frame >= 0xF && (s16)frame_1A < 0xF) {
            int step;
            g_RoomM273SwayState.sway_timer = 8;
            value = (s16)FieldFx_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation, (Vec3Fixed *)instance->position);
            step = 0x20;
            if ((value - instance->yaw) & 0x800) step = -0x20;
            g_RoomM273SwayState.sway_step = step;
        }
        if (g_RoomM273SwayState.sway_timer) {
            instance->yaw += g_RoomM273SwayState.sway_step *
                             rcossin_tbl[(g_RoomM273SwayState.sway_timer << 7) & 0xF80].bits.sin / 4096;
            g_RoomM273SwayState.sway_timer--;
        }
        if (g_RoomM273SwayState.cooldown) g_RoomM273SwayState.cooldown--;
    }
    return 0;
}

short *RoomM273_GrabEffectConfigure(void *arg0, void *arg1, short arg2) {
    short *ptr = &g_RoomM273GrabColorIndex;

    *ptr = arg2;
    return ptr - 6;
}

int RoomM273_GrabSparkTriangleCallback(int mode, RoomM273PulseState *state) {
    RoomM273Vector position, rotation;
    s32 frame;
    s32 *source;
    s32 positionX;
    int scaleX, scaleY;
    char *texture;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 9) return 1;
        state->value += state->step;
    } else if (mode == 2) {
        source = state->source;
        positionX = source[0];
        position.y = source[1];
        position.z = source[2];
        rotation.x = 0;
        rotation.y = state->value;
        rotation.z = 0;
        frame = g_FieldFxCurrentAge - 1;
        scaleX = state->scaleX;
        scaleY = state->scaleY;
        texture = g_RoomM273BlackColor;
        position.x = positionX;
        FieldFx_DrawRotatedTriangle(&position, (GteRotation *)&rotation, scaleX, scaleY, (RenderColor *)state,
                       (RenderColor *)texture, (RenderColor *)texture,
                       rcossin_tbl[(frame << 7) & 0xF80].bits.cos >> 5, 1);
    }
    return 0;
}


s32 RoomM273_GrabHoldFlareEmitter(s32 state) {
    GteShortVector *obj;
    RoomM273PlayerActorView *root;
    RoomM273ThresholdTransform *source;
    u8 status;

    if (state != 1) {
        if (state >= 2) {
            return 0;
        }
        if (state != 0) {
            return 0;
        }
        g_RoomM273HoldFlareSpawned = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 5,
                             (FieldFxCallbackListCallback)RoomM273_GrabHoldFlareCallback);
    }
    if (g_RoomM273HoldFlareSpawned != 0) {
        return 2;
    }

    root = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.player;
    status = root->kind;
    if (status != 0x10 && status != 8) {
        g_RoomM273HoldFlareSpawned = 1;
        return 2;
    }
    if ((g_FieldFxCurrentAge & 3) != 0) {
        return 0;
    }

    obj = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM273PoolContext)->pool);
    if (obj != 0) {
        root = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.player;
        source = (RoomM273ThresholdTransform *)root->tail.object;
        obj->x = source->x;
        obj->y = source->y;
        obj->z = source->z;
    }
    return 0;
}

void *RoomM273_HandTrailConfigure(void *unused, int mode) {
    int direction;
    int value;

    g_RoomM273HandTrailState.spinning = mode;
    switch (mode) {
    case 0:
        g_RoomM273HandTrailState.spin_target = 0;
        g_RoomM273HandTrailState.repeat = 2;
        break;
    case 1:
        direction = (Inv_ScrambleGrid() & 1) ? 42 : -42;
        g_RoomM273HandTrailState.spin_step = 2;
        g_RoomM273HandTrailState.repeat = 2;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0x61;
        g_RoomM273HandTrailState.spin_target = direction;
        g_RoomM273HandTrailState.tick_limit = value;
        break;
    case 2:
        direction = (Inv_ScrambleGrid() & 1) ? 0x80 : -0x80;
        g_RoomM273HandTrailState.spin_step = 4;
        g_RoomM273HandTrailState.repeat = 5;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0xB0;
        g_RoomM273HandTrailState.spin_target = direction;
        g_RoomM273HandTrailState.tick_limit = value;
        break;
    }
    return &g_RoomM273HandTrailState;
}

int RoomM273_HandTrailLandingFanCallback(int mode, RoomM273PhaseState *state) {
    RoomM273Vector position;
    int frame, size;
    RoomM273TrigEntry *entry;

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (mode == 2) {
        frame = g_FieldFxCurrentAge - 1;
        entry = &rcossin_tbl[((frame << 8) & 0x3F00) >> 2];
        size = (s16)(entry->low * 2 + 2048);
        position.x = state->source[0];
        position.y = g_RoomFloorY->raw;
        position.z = state->source[2];
        FieldFx_DrawStarFan(&position, 192, 192, 10,
                       (GteRotation *)(g_RoomM273LandingFanRotations + ((frame & 1) << 3)), size, size,
                       (RenderColor *)g_RoomM273BlackColor, (RenderColor *)g_RoomM273LandingFanColor, entry->high >> 5, 1);
    }
    return 0;
}
