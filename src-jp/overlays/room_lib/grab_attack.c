/*
 * The grab attack set: an actor class that seizes the player when it comes
 * within reach, holds the player through its own animation and lets go,
 * followed by the effects its attacks use: a ground sweep with its scorch
 * marks, an orbit trail burst and its particles, a rising spray and its
 * sparks, and the setters of the room's attack arguments.
 *
 * room_m269 and scene_e01 link the same twenty functions in this order at
 * the same addresses, from the class's first no-op to the argument setter,
 * with their read-only data (the hold scale, the mark seed, the sweep's
 * offset, colours and jump table, the trail and burst colours and the spray
 * offset) as one contiguous block after the overlay header; scene_e01 is the
 * room's code and data cut short after the module. This unit is that
 * object, compiled into both. The attack arguments, the sweep's side, the
 * orbit centre and the trail's track are each overlay's own data
 * (pe1/room_grab_attack.h).
 */

#include "pe1/room_grab_attack.h"

/* The player, as the class sees it. */
#define GRAB_PLAYER ((FieldActor *)D_8009CD10)

/* Class slots 0 and 3. */
int RoomEffect_GrabNop0(void) {
    return 0;
}

int RoomEffect_GrabInit(RoomMotionTrigger *obj) {
    obj->callback = RoomEffect_GrabAwaitTarget;
    obj->flag03 = 0xFF;
    obj->flag1A = 0;
    obj->activated = 0;
    return 0;
}

/* Message handler: message 0x19 hands the class a completion flag, message
 * 0x200 finds the actor it grabs with. */
int RoomEffect_GrabConfigure(char *obj, int arg1, int op, int arg3, int arg4) {
    FieldActor *node;
    RoomGrabStateTail *base;
    FieldActor *next;

    base = (RoomGrabStateTail *)(obj + 0xC);
    if (op != 0x19) {
        if (op != 0x200) {
            return 0;
        }
    } else {
        if (arg1 == 1) {
            ((RoomMotionTrigger *)obj)->completion_state = (s32 *)arg3;
            *(int *)arg3 = arg1;
        }
        return 0;
    }

    next = g_FieldActorListHead;
    ((RoomMotionTrigger *)obj)->source_actor = next;
    if (next != 0) {
        do {
            node = base->source_actor;
            if (node->type_id == arg3) {
                if (node->sub_id == arg4) {
                    if ((node->flags & 0x10) == 0) {
                        return 0;
                    }
                }
            }
            next = base->source_actor->next;
            base->source_actor = next;
        } while (next != 0);
    }

    return 0;
}

int RoomEffect_GrabNop3(void) {
    return 0;
}

/* Class update: runs the current state. */
int RoomEffect_GrabUpdate(RoomMotionTrigger *obj) {
    obj->callback();
    return 0;
}

/* Waits until the actor reaches its attack stance. */
void RoomEffect_GrabAwaitTarget(RoomMotionTrigger *obj) {
    if (obj->probe_actor->mode == 0x10) {
        obj->callback = RoomEffect_GrabStartOnProximity;
    }
}

void RoomEffect_GrabStartOnProximity(RoomMotionTrigger *arg) {
    u8 mode = GRAB_PLAYER->mode;
    s32 kind = arg->probe_actor->anim.parts.integer;

    if (mode >= 0x12) {
        RoomEffect_GrabRelease(arg);
        return;
    }
    if (kind < 8) {
        return;
    }
    if (kind < 0xF) {
        RoomGrabPoint *sp = (RoomGrabPoint *)0x1F800008;
        s32 v;
        s32 s;
        /* Reuse the distance result scratch for the negative sine below. */
        s32 result;
        FieldActor *p;
        RenderMatrix *rotationMatrix;
        FieldActor *actor;

        Render_FindAnimEntry(&arg->source_actor->render_object, 6, sp);
        sp->x <<= 16;
        sp->z <<= 16;
        result = FieldFx_GetDistanceXZ((Vec3Fixed *)&GRAB_PLAYER->pos_x, (Vec3Fixed *)sp);
        if (result >= 0x100) {
            return;
        }
        arg->callback = RoomEffect_GrabHold;
        Battle_BeginPlayerAction();

        actor = GRAB_PLAYER;
        actor->render_object.animation_source = &arg->source_actor->render_object;
        actor->render_object.animation_state = 4;
        actor->render_object.animation_id = 6;
        actor->flags |= 0x10000;
        actor->render_object.flags_9C |= 0x400;
        s = rcos(0xC00);
        v = rsin(0xC00);
        result = -v;

        p = GRAB_PLAYER;
        rotationMatrix = &p->render_object.model_matrix;
        rotationMatrix->rotation[1][0] = v;
        v = 0x1000;
        rotationMatrix->rotation[2][2] = v;
        *(volatile s32 *)&rotationMatrix->translation[0] = 0;
        v = 0x100;
        rotationMatrix->translation[0] = v;
        *(volatile s32 *)&rotationMatrix->translation[2] = 0;
        v = -0xC0;
        rotationMatrix->translation[2] = v;
        rotationMatrix->rotation[0][0] = s;
        rotationMatrix->rotation[0][1] = result;
        rotationMatrix->rotation[1][1] = s;
        rotationMatrix->translation[1] = 0;
        rotationMatrix->rotation[2][1] = 0;
        rotationMatrix->rotation[2][0] = 0;
        rotationMatrix->rotation[1][2] = 0;
        rotationMatrix->rotation[0][2] = 0;
        rotationMatrix->translation[1] = 0;
        arg->activated = 1;

        p = GRAB_PLAYER;
        arg->saved_x = p->pos_x;
        arg->saved_z = p->pos_z;
        return;
    }
    RoomEffect_GrabRelease(arg);
}

static const RoomFxVec4 s_GrabHoldScale = { 0x1400, 0x1400, 0x1400, 0 };

/* The grab animation uses the field actor view of the player record. */
static __inline__ FieldActor *RoomEffect_GrabPlayerView(RoomSparkBattleEntity *player) {
    return (FieldActor *)player;
}

void RoomEffect_GrabHold(RoomMotionTrigger *arg0) {
    FieldActor *s1 = arg0->probe_actor;
    FieldActor *g;
    RenderMatrix *rotationMatrix;
    RoomFxVec4 scale;
    s32 dc4, cf4;
    s32 neg;
    s32 s0;
    /* PIN-DEBT: retail keeps a second live copy of the animation frame in $s4; without the
     * pin GCC copy-propagates s4=s0 away and drops the callee-saved slot. */
    register s32 s4 asm("$20");
    s32 kind;
    s32 val;
    /* Keep the previous animation frame unsigned until each comparison. */
    u16 raw_val;
    s32 signed_val;
    /* PIN-DEBT: the current-frame comparison still needs $v0. */
    register s32 signed_s4 asm("$2");
    FieldActor *matrix_arg;
    s32 *p;
    void *s3v = &arg0->callback;

    dc4 = rcos(0xC00);
    cf4 = rsin(0xC00);
    g = RoomEffect_GrabPlayerView(((RoomSparkBattleEntity *)D_8009CD10));
    rotationMatrix = &g->render_object.model_matrix;
    neg = -cf4;
    rotationMatrix->rotation[0][1] = neg;
    rotationMatrix->rotation[1][0] = cf4;
    rotationMatrix->rotation[0][0] = dc4;
    rotationMatrix->rotation[1][1] = dc4;
    rotationMatrix->translation[2] = 0;
    rotationMatrix->translation[1] = 0;
    rotationMatrix->translation[0] = 0;
    rotationMatrix->rotation[2][1] = 0;
    rotationMatrix->rotation[2][0] = 0;
    rotationMatrix->rotation[1][2] = 0;
    rotationMatrix->rotation[0][2] = 0;
    rotationMatrix->rotation[2][2] = 0x1000;
    scale = s_GrabHoldScale;
    matrix_arg = RoomEffect_GrabPlayerView(((RoomSparkBattleEntity *)D_8009CD10));
    ScaleMatrix(&matrix_arg->render_object.model_matrix, &scale);
    g = RoomEffect_GrabPlayerView(((RoomSparkBattleEntity *)D_8009CD10));
    g->render_object.model_matrix.translation[0] = 0x100;
    g->render_object.model_matrix.translation[1] = -0x20;
    g->render_object.model_matrix.translation[2] = -0xC0;
    s0 = s1->anim.parts.integer;
    g->pos_x = arg0->saved_x;
    g->pos_z = arg0->saved_z;
    s4 = s0;

    if (s1->state == 0 || Battle_GetEnemyContextField(s1, 0x2C) <= 0) {
        if (RoomEffect_GrabPlayerView(((RoomSparkBattleEntity *)D_8009CD10))->state->amount > 0) {
            Battle_ReturnToIdle();
        }
        RoomEffect_GrabResetPlayer(s1, s3v);
        RoomEffect_GrabRelease(arg0);
        return;
    }

    if (RoomEffect_GrabPlayerView(((RoomSparkBattleEntity *)D_8009CD10))->state->amount <= 0) {
        RoomEffect_GrabResetPlayer(s1, s3v);
        RoomEffect_GrabRelease(arg0);
        return;
    }
    kind = s1->mode;
    if (kind != 7) {
    if (kind < 8) {
        if (kind == 6) {
            return;
        }
        Battle_ReturnToIdle();
        RoomEffect_GrabResetPlayer(s1, s3v);
        RoomEffect_GrabRelease(arg0);
        return;
    }

    if (kind != 0x10) {
        Battle_ReturnToIdle();
        RoomEffect_GrabResetPlayer(s1, s3v);
        RoomEffect_GrabRelease(arg0);
        return;
    }
    if ((s16)s0 < 0x12) {
        return;
    }
    p = arg0->completion_state;
    if (p != 0) {
        *p = 2;
    }
    return;
    }

    val = ((u16 *)&s1->anim_prev)[1];
    /* The barrier retains the raw load and the two separate sign extensions;
     * its memory clobber makes the flag store below reload the player and
     * its state, as retail does. */
    raw_val = val;
    asm volatile("" : "=r"(raw_val) : "0"(raw_val) : "memory");
    if ((s16)val >= 0xE || (s16)s0 < 0xE) {
        signed_val = (s16)raw_val;
        if (signed_val >= 3) {
            return;
        }
        signed_s4 = (s16)s4;
        if (signed_s4 < 3) {
            return;
        }
        {
            /* One load of the player's state for the read and the write. */
            FieldActorState *state = RoomEffect_GrabPlayerView(((RoomSparkBattleEntity *)D_8009CD10))->state;

            state->flags |= 0x4000;
        }
        if (s1->state != 0) {
            s1->state->core_flags |= 0x80000000;
        }
        return;

    }
    Battle_ReturnToIdle();
    RoomEffect_GrabResetPlayer(s1, s3v);
    RoomEffect_GrabRelease(arg0);
}

/* Lets the player go: clears the animation borrowed from the actor, puts
 * the player back where the actor's hold ends and stops its motion. Called
 * with the class's state words from 0x0C on. */
void RoomEffect_GrabResetPlayer(FieldActor *unused, RoomGrabStateTail *state) {
    {
        FieldActor *actor = GRAB_PLAYER;

        actor->render_object.animation_source = 0;
        actor->render_object.animation_state = 0;
        actor->flags &= 0xFFFEFFFF;
        actor->render_object.flags_9C &= 0xFBFF;
        Render_FindAnimEntry(&state->source_actor->render_object, 6, &actor->pos_x);
    }

    {
        FieldActor *actor = GRAB_PLAYER;

        actor->pos_x <<= 16;
        actor->pos_z <<= 16;
        Entity_FindFloor(actor);
    }

    {
        FieldActor *actor = GRAB_PLAYER;
        int x = actor->pos_x;
        int y = actor->pos_y;
        int z = actor->pos_z;
        int *flags = (int *)&D_800BCA48;

        actor->motion_x = 0;
        actor->motion_y = 0;
        actor->motion_z = 0;
        actor->accel_x = 0;
        actor->accel_y = 0;
        actor->accel_z = 0;
        actor->base_x = x;
        actor->base_y = y;
        actor->base_z = z;
        *flags |= 0x80;
        state->activated = 0;
    }
}

/* Class release: signals completion and, if the player is held, lets go. */
int RoomEffect_GrabRelease(RoomMotionTrigger *entity) {
    int *signal;

    signal = entity->completion_state;
    if (signal != 0) {
        *signal = 0;
    }

    entity->state = 4;
    if (entity->activated != 0) {
        {
            FieldActorState *child = GRAB_PLAYER->state;

            if (child->amount > 0) {
                Battle_ReturnToIdle();
            }
        }

        {
            FieldActor *actor = GRAB_PLAYER;

            actor->render_object.animation_source = 0;
            actor->render_object.animation_state = 0;
            actor->flags &= 0xFFFEFFFF;
            actor->render_object.flags_9C &= 0xFBFF;
            Render_FindAnimEntry(&entity->source_actor->render_object, 6, &actor->pos_x);
        }

        {
            FieldActor *actor = GRAB_PLAYER;

            actor->pos_x <<= 16;
            actor->pos_z <<= 16;
            Entity_FindFloor(actor);
        }

        {
            volatile FieldActor *actor;
            int x;
            int y;
            int z;

            actor = GRAB_PLAYER;
            x = actor->pos_x;
            y = actor->pos_y;
            z = actor->pos_z;
            actor->motion_x = 0;
            actor->motion_y = 0;
            actor->motion_z = 0;
            actor->accel_x = 0;
            actor->accel_y = 0;
            actor->accel_z = 0;
            actor->base_x = x;
            actor->base_y = y;
            actor->base_z = z;
            D_800BCA48 |= 0x80;
            entity->activated = 0;
        }
    }

    return 0;
}

/* Class slot 6. */
int RoomEffect_GrabNop6(void) {
    return 0;
}

static const RoomFxSeed8 s_GroundSweepMarkSeed = {{0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}};

int RoomEffect_GroundSweepMark(int mode, RoomGroundSweepMark *state) {
    GteShortVector position;
    RoomFxSeed8 seed = s_GroundSweepMarkSeed;
    int shade;
    int palette;
    int kind;
    u16 clut;

    switch (mode) {
    case 1: {
        int next;
        int signed_next;
        if (state->blocked) return 0;
        next = state->frame + 1;
        state->frame = next;
        /* Keep the halfword store before the signed range check. */
                signed_next = (s16)next;
        if (signed_next < 16) return 0;
        /* The two return paths must stay distinct. */
        asm volatile("" ::: "memory");
        return 1;
    }
    case 2:
        break;
    default:
        return 0;
    }
    if (state->blocked) return 0;

    shade = (g_FieldFxCurrentAge & 1) ? 128 : 100;
    position.x = state->x;
    position.y = state->y;
    position.z = state->z;

    if ((s16)state->frame < 8) {
        int first_kind;
        first_kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[first_kind];
        if (first_kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(16, palette);
        FieldFx_DrawShapeQuads(&position, (GteRotation *)&seed, 4096, 4096,
                        (s16)g_FieldFxSpriteParams.parameter02 * (s16)state->frame + 32,
                        clut, 1, (unsigned)shade >> 1, 0);
    }

    position.y -= 40;
    kind = g_FieldFxSpriteParams.palette;
    palette = g_FieldFxPaletteTable[kind];
    if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
    clut = GetClut(48, palette);
    FieldFx_DrawShapeQuads(&position, 0, 4096, 4096,
                    (s16)g_FieldFxSpriteParams.parameter02 * ((s16)state->frame / 2) + 96,
                    clut, 1, (unsigned)shade >> 1, 0);
    return 0;
}


static const GteShortVector s_GroundSweepOffset = { 0, 0, -80, 0 };
static const RenderColor s_GroundSweepColorA = { 0x50, 0x00, 0xA0, 0x00 };
static const RenderColor s_GroundSweepColorB = { 0x00, 0x50, 0xA0, 0x00 };

/* Controller that sweeps a ribbon of light along the ground from the
 * actor's hand: it rises, lunges out and back a few times leaving scorch
 * marks and hitting the player in reach, then fades out. */
int RoomEffect_GroundSweepController(int mode, RoomGroundSweep *sweep,
                                 RoomGroundSweepParams *params) {
    GteShortVector offset = s_GroundSweepOffset;
    GteShortVector hand;
    GteShortVector step;
    GteShortVector work;          /* hand rotation, then the blended tip */
    GteShortVector angles;
    RenderColor colorA = s_GroundSweepColorA;
    RenderColor colorB = s_GroundSweepColorB;
    RoomGroundSweepMark *mark;
    int scale;

    switch (mode) {
    case 0:
        g_RoomGrabSweepSide = 2 - g_RoomGrabSweepSide;
        work.y = 0;
        work.x = (g_RoomGrabSweepSide - 1) * 350;
        work.z = -(rand() & 0xFF) - 400;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &work, sweep);
        sweep->y = g_RoomFloorY->y;
        sweep->x += (rand() & 0x1F) - 0x10;
        sweep->z += (rand() & 0x1F) - 0x10;
        sweep->heading.x = 0;
        sweep->heading.y = rand() & 0xFFF;
        sweep->heading.z = 0;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &offset, &hand);
        FieldFx_DrawHistoryTrail(&hand, (GteShortVector *)sweep, 0x3F0, (unsigned char *)&colorA, (unsigned char *)&colorA, 0x8C, (struct FieldTrailPair *)sweep->trail, 1);
        if (FX_TASK_AS(RoomSparkEventState)->active) {
            RoomSparkNode **slot = (RoomSparkNode **)ANIM_OWNER_AS(RoomSparkChannel)->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        sweep->phase = 0;
        sweep->timer = 0;
        sweep->sweeps = 8;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0xC, 0x10,
                             (FieldFxCallbackListCallback)RoomEffect_GroundSweepMark);
    case 1:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, (s16 *)&hand);
        hand.y = g_RoomFloorY->y;
        switch (sweep->phase) {
        case 0:
            sweep->timer++;
            if ((s16)sweep->timer == 2) {
                RoomSoundSlot *sound = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
                sweep->soundHandle = sound->channel
                    ? Akao_SendTableCommand(sound->channel, 0x598, FieldFx_GetOwnerProgress(), 0x80, 0x7F)
                    : 0;
            }
            if ((s16)sweep->timer < 4) break;
            sweep->phase = 1;
            sweep->timer = 0;
            break;
        case 1: {
            int half;
            sweep->timer++;
            scale = rsin(((s16)sweep->timer << 11) / 12);
            half = params->distance / 2;
            scale = scale * half / 4096;
            scale += half;
            FieldFx_AnglesToVector(&sweep->heading, (s16)scale, &step);
            sweep->x += step.x;
            sweep->y += step.y;
            sweep->z += step.z;
            sweep->y = g_RoomFloorY->y;
            mark = (RoomGroundSweepMark *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (mark) {
                mark->x = sweep->x;
                mark->y = sweep->y;
                mark->z = sweep->z;
                mark->blocked = 0;
                mark->frame = 0;
            }
            if (FieldFx_IsPlayerWithinRadius(sweep, 50)) {
                if (FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    if ((((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        {
                            RoomSparkActor *player = ((RoomSparkBattleEntity *)D_8009CD10)->actor;

                            player->flags |= 0x4000;
                        }
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags =
                            (((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x3D000000;
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
            }
            if ((s16)sweep->timer < 12) break;
            sweep->phase = 2;
            if (--sweep->sweeps <= 0) sweep->phase = 3;
            sweep->timer = 0;
            break;
        }
        case 2:
            sweep->timer++;
            scale = (s16)sweep->timer * 650;
            FieldFx_CalculateLookAngles(sweep, &hand, &angles);
            angles.x = 0;
            FieldFx_EaseAngles((unsigned short *)&angles, (unsigned short *)&sweep->heading, (u16)scale);
            FieldFx_AnglesToVector(&sweep->heading, (s16)(params->distance / 2), &step);
            sweep->x += step.x;
            sweep->y += step.y;
            sweep->z += step.z;
            sweep->y = g_RoomFloorY->y;
            mark = (RoomGroundSweepMark *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (mark) {
                mark->x = sweep->x;
                mark->y = sweep->y;
                mark->z = sweep->z;
                mark->blocked = 0;
                mark->frame = 0;
            }
            if (FieldFx_IsPlayerWithinRadius(sweep, 50)) {
                if (FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    if ((((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        {
                            RoomSparkActor *player = ((RoomSparkBattleEntity *)D_8009CD10)->actor;

                            player->flags |= 0x4000;
                        }
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags =
                            (((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x3D000000;
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
            }
            if ((s16)sweep->timer < 4) break;
            sweep->phase = 1;
            if (--sweep->sweeps <= 0) sweep->phase = 3;
            sweep->timer = 0;
            break;
        case 3:
            sweep->timer++;
            if ((s16)sweep->timer < 8) break;
            sweep->phase = 4;
            sweep->timer = 0;
            if (sweep->soundHandle != -1) Akao_Cmd_21(sweep->soundHandle, 0);
            break;
        case 4:
            sweep->timer++;
            if ((s16)sweep->timer < 0x39) break;
            return 2;
        }
        break;
    case 2: {
        RenderColor *color;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &offset, &hand);
        if (g_FieldFxCurrentAge & 1) color = &colorA;
        else color = &colorB;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.depth = 0xC;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        switch (sweep->phase) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            scale = (s16)sweep->timer << 10;
            LoadAverageShort12(&hand, sweep, 0x1000 - scale, scale, &work);
            FieldFx_DrawGlowLine(&hand, &work, color, color, 0x80, 0, 1);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads(&hand, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, 0x80, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            FieldFx_DrawGlowLine(&hand, sweep, color, color, 0x8C, 0x8C, 1);
            FieldFx_DrawHistoryTrail(&hand, (GteShortVector *)sweep, 8, (unsigned char *)color, (unsigned char *)&colorA, 0x8C, (struct FieldTrailPair *)sweep->trail, 1);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads(&hand, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, 0x80, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            FieldFx_DrawGlowLine(&hand, sweep, color, color, 0x60, 0x60, 1);
            FieldFx_DrawHistoryTrail(&hand, (GteShortVector *)sweep, 8, (unsigned char *)color, (unsigned char *)&colorA, 0x8C, (struct FieldTrailPair *)sweep->trail, 1);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads(&hand, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, 0x80, 0);
            break;
        }
        case 3: {
            u16 clut;
            int kind;
            int palette;
            int fade = rcos((s16)sweep->timer << 7) / 32;
            FieldFx_DrawGlowLine(&hand, sweep, color, color, fade, fade, 1);
            FieldFx_DrawHistoryTrail(&hand, (GteShortVector *)sweep, 8, (unsigned char *)color, (unsigned char *)&colorA, 0x8C, (struct FieldTrailPair *)sweep->trail, 1);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads(&hand, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, fade, 0);
            break;
        }
        case 4:
            break;
        }
        break;
    }
    }
    return 0;
}


static const RenderColor s_OrbitTrailColor = { 0x80, 0x80, 0x80, 0x00 };

/* Particle that sweeps out from the orbit centre drawing a coloured trail,
 * then flashes as a pulsing sprite, then drifts as a tilted ring; the
 * update side drifts and bounces it while it fades. */
int RoomEffect_OrbitTrailParticle(int mode, RoomOrbitTrailParticle *p) {
    GteShortVector offset;
    GteShortVector blend;
    RenderColor color;
    int fall;
    int bounce;
    int scale;
    int weight;
    int glow;

    color = s_OrbitTrailColor;
    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.x += 0x10;
            p->heading.y += 8;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->heading.z += 0x80;
            if ((s16)p->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            scale = (rsin(((s16)p->timer << 10) / 24) + 0x1000) / 2;
            FieldFx_AnglesToVector(&p->heading, (s16)(p->radius * scale / 4096), &offset);
            offset.x += g_RoomGrabOrbitCentre.x;
            offset.y += g_RoomGrabOrbitCentre.y;
            offset.z += g_RoomGrabOrbitCentre.z;
            weight = rsin(((s16)p->timer << 10) / 24);
            LoadAverageShort12(&g_RoomGrabOrbitCentre, &offset, 0x1000 - weight,
                          weight, &blend);
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            FieldFx_SampleColorTrack(g_RoomGrabTrailTrack, &color, (s16)p->timer);
            FieldFx_DrawGlowLine(&blend, &offset, 0, &color, 0, 0x80, 1);
            break;
        case 1: {
            u16 clut;
            int kind;
            int palette;
            int size = rsin((s16)p->timer << 6) / 2 + 0x1000;
            glow = rcos((s16)p->timer * 1204 / 16) / 128 + 0x28;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)p, 0, size, size,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)p->timer / 2),
                          clut, 1, glow, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            glow = rcos((s16)p->timer << 7) / 32;
            scale = rsin((s16)p->timer << 7);
            /* The scaled radius shares the palette kind's temporary, which
             * is what leaves the heading pointer the first saved register. */
            kind = p->radius * scale / 4096;
            FieldFx_AnglesToVector(&p->heading, (s16)kind, &offset);
            offset.x += g_RoomGrabOrbitCentre.x;
            offset.y += g_RoomGrabOrbitCentre.y;
            offset.z += g_RoomGrabOrbitCentre.z;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            *(u32 *)&color = 0x808080;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x90, palette);
            FieldFx_DrawRotatedQuad(&offset, (GteRotation *)&p->heading, 600, 100, 0x70, 0xA0,
                          0x40, 0x20, clut, &color, &color, (s16)glow, 1);
            break;
        }
        }
        break;
    }
    return 0;
}


static const RenderColor s_OrbitBurstColor = { 0xC8, 0xBE, 0x78, 0x00 };

/* Controller that bursts orbit trail particles from the actor during the
 * first four frames (falling sparks, sweeping trails and drifting rings),
 * flashes a glow, ring and halo while it fades, and publishes the actor
 * position as the orbit centre the trails sweep out from. */
int RoomEffect_OrbitTrailBurst(int mode, RoomOrbitTrailBurst *burst) {
    RenderColor color = s_OrbitBurstColor;
    RoomOrbitTrailParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->counterB = 0;
        burst->counterA = 0;
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, (s16 *)burst);
        FieldFx_PlaySoundAtOwner(0x596, FieldFx_GetOwnerProgress());
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)RoomEffect_OrbitTrailParticle);
    case 1:
        if (g_FieldFxCurrentAge < 4) {
            for (i = 0; i < 3; i++) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = rand() % 70 - 0x23;
                    child->heading.y = rand() % 70 - 0x23;
                    child->heading.z = rand() % 70 - 0x23;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 5; i++) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = rand();
                    child->heading.y = rand();
                    child->heading.z = rand();
                    child->radius = rand() % 800 + 0x200;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 4; i++) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = rand();
                    child->heading.y = rand();
                    child->heading.z = rand();
                    child->radius = (rand() & 0x1FF) + 0x100;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 0x1C) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x20;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        if (g_FieldFxCurrentAge < 0x15) {
            u16 clut;
            int kind;
            int palette;
            int glow = rcos((g_FieldFxCurrentAge << 10) / 20) / 32;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x10, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)burst, 0, 0x2000, 0x2000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 3) + 0x20,
                          clut, 1, glow, 0);
            FieldFx_DrawStarFan((GteShortVector *)burst, 500, 500, 12, 0, 0x1000, 0x1000,
                          &color, 0, glow, 1);
            {
                int scale = rsin((g_FieldFxCurrentAge << 10) / 20) / 4 + 0x1000;
                FieldFx_DrawRingBand((GteShortVector *)burst, 0x15E, 0x190, 0x18, 0, scale,
                              scale, 0, &color, glow / 2, 1);
            }
        }
        g_RoomGrabOrbitCentre.x = burst->x;
        g_RoomGrabOrbitCentre.y = burst->y;
        g_RoomGrabOrbitCentre.z = burst->z;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 8;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}


/* Spray spark: rises along a swirling path and knocks the player back when
 * it reaches them, then hovers, or falls and bounces off the floor; drawn
 * as a spinning glow with a floor shadow, a fading glow or a shrinking
 * spark. */
int RoomEffect_RisingSprayParticle(int mode, RoomSpraySpark *spark) {
    GteRotation rotation;
    GteShortVector position;
    GteShortVector shadow;
    GteRotation shadowRotation;
    int fall;
    int bounce;
    int width;
    int height;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->y += rsin((s16)spark->timer * 292) / 256;
            spark->swirl += 200;
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.x += rcos(spark->swirl) / 128;
            position.z += rsin(spark->swirl) / 128;
            if (FieldFx_IsPlayerWithinRadius(&position, 100)) {
                if (FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    if ((((RoomSprayHitPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        {
                            RoomSparkActor *player = ((RoomSparkBattleEntity *)D_8009CD10)->actor;

                            player->flags |= 0x4000;
                        }
                        ((RoomSprayHitPool *)channel->pool)->object->flags =
                            (((RoomSprayHitPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x19000000;
                        ((RoomSprayHitPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
                spark->vx = 0;
                spark->vy = 0;
                spark->vz = 0;
                spark->state = 1;
                spark->timer = 0;
            }
            if ((s16)spark->timer >= 0x22) {
                spark->state = 1;
                spark->timer = 0;
                spark->vx /= 2;
                spark->vy /= 2;
                spark->vz /= 2;
            }
            if (Geo_PointInPoly(spark->x << 16, spark->z << 16, g_FloorPolygonVertices, g_FloorPolygonVertexCount)) break;
            return 1;
        case 1:
            spark->timer++;
            spark->y -= 2;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            if ((s16)spark->timer < 0x10) break;
            return 1;
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        switch (spark->state) {
        case 0: {
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)spark->timer * 32;
            rotation.flags = 0;
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.x += rcos(spark->swirl) / 128;
            position.z += rsin(spark->swirl) / 128;
            width = rcos((s16)spark->timer * 384) / 4 + 0xC00;
            width *= 2;
            height = rsin((s16)spark->timer * 384) / 4 + 0xC00;
            height *= 2;
            {
                u16 clut;
                int kind;
                int palette;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x70, palette);
                FieldFx_DrawShapeQuads(&position, &rotation, width, height, 0xA3, clut, 1,
                              0x80, 0);
            }
            shadowRotation.x = 0x400;
            shadowRotation.y = 0;
            shadowRotation.z = (s16)spark->timer * 80;
            shadowRotation.flags = 1;
            shadow.x = position.x;
            shadow.z = position.z;
            shadow.y = g_RoomFloorY->y;
            {
                u16 clut;
                int kind;
                int palette;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x70, palette);
                FieldFx_DrawShapeQuads(&shadow, &shadowRotation, width, height, 0xA0, clut, 2,
                              0x30, 0);
            }
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.x += rcos(spark->swirl) / 128;
            position.z += rsin(spark->swirl) / 128;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x70, palette);
            FieldFx_DrawShapeQuads(&position, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)spark->timer / 2) + 0x80,
                          clut, 1, 0x80, 0);
            break;
        }
        case 2: {
            int t = (s16)spark->timer;
            height = t - 16;
            if (t <= 16) {
                u16 clut;
                int kind;
                int palette;
                width = rsin(t << 6) / 2 + 0x800;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x70, palette);
                FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, width, width, 0xA3, clut, 1, 0x80, 0);
            } else {
                u16 clut;
                int kind;
                int palette;
                height /= 2;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x70, palette);
                FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, 0xAAA, 0xAAA,
                              (s16)g_FieldFxSpriteParams.parameter02 * height + 0x80,
                              clut, 1, 0x80, 0);
            }
            break;
        }
        }
        break;
    }
    return 0;
}


static const GteShortVector s_RisingSprayOffset = { 0, 0, -150, 0 };

/* Controller that sprays sparks from above the primary actor: a wide burst
 * on odd frames for the first second, then a stream along the actor's
 * heading, with two sound cues at frames 1 and 16. */
int RoomEffect_RisingSprayController(int mode, void *state,
                                 RoomRisingSprayParams *params) {
    GteShortVector offset = s_RisingSprayOffset;
    GteShortVector position;
    GteShortVector heading;
    GteShortVector angles;
    RoomDampedSpark *child;

    switch (mode) {
    case 0:
        if (FX_TASK_AS(RoomSparkEventState)->active) {
            RoomSparkNode **slot = (RoomSparkNode **)ANIM_OWNER_AS(RoomSparkChannel)->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x14, 0x37,
                             (FieldFxCallbackListCallback)RoomEffect_RisingSprayParticle);
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, &offset, &position);
        position.y -= 0x50;
        FieldFx_GetTransformLookAngles((void *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, &heading);
        heading.z = 0;
        heading.x = 0;
        if ((g_FieldFxCurrentAge & 1) && g_FieldFxCurrentAge < 0x3C) {
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                angles.x = heading.x;
                angles.y = heading.y;
                angles.z = heading.z;
                angles.x += (rand() & 0xFF) - 0x80;
                angles.y += (rand() & 0x3FF) - 0x200;
                FieldFx_AnglesToVector(&angles, (rand() & 0xF) | 0x10,
                              (GteShortVector *)&child->vx);
                child->state = 2;
                child->timer = 0;
            }
        }
        if (g_FieldFxCurrentAge == 1) {
            RoomSoundSlot *sound = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
            if (sound->channel) {
                Akao_SendTableCommand(sound->channel, 0x599, FieldFx_GetOwnerProgress(), 0x80, 0x7F);
                if (sound->channel) Akao_SendTableCommand(sound->channel, 0x5B5, 0x80, 0x80, 0x7F);
            }
        }
        if (g_FieldFxCurrentAge == 0x10) {
            RoomSoundSlot *sound = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
            if (sound->channel) {
                Akao_SendTableCommand(sound->channel, 0x5B6, FieldFx_GetOwnerProgress(), 0x80, 0x7F);
                if (sound->channel) Akao_SendTableCommand(sound->channel, 0x5B7, 0x80, 0x80, 0x7F);
            }
        }
        if ((unsigned)(g_FieldFxCurrentAge - 0x10) < 0x2D) {
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                heading.y += rand() % params->spread - params->spread / 2;
                FieldFx_AnglesToVector(&heading, (s16)(params->speed + (rand() & 0xF)),
                              (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
        }
        if (g_FieldFxCurrentAge < 2) break;
        return 2;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 4;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Setters of the room's attack arguments, called from its script. */
int *RoomEffect_GrabSetSlot(int a0, int a1) {
    int *p = &g_RoomGrabSlot;
    *p = a1;
    return p;
}

int *RoomEffect_GrabGetSlot(void) {
    return &g_RoomGrabSlot;
}

int *RoomEffect_GrabSetPair(int a0, int a1, int a2) {
    int *p = &RoomLib_PairA;
    *p = a1;
    RoomLib_PairB = a2;
    return p;
}
