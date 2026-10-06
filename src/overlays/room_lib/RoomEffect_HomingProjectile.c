/*
 * The thrown homing projectile: its spark callback, the flight itself, the
 * throw controller that drives the actor's throw animation and seeds the
 * model, and the entry the scene script sets the launch parameters with.
 *
 * room_m141, room_m146, room_m153, room_m154, room_m328 and scene_e02 link
 * these four functions in this order as the last code of the overlay, with
 * the same 0x28 bytes of seeds in their data; this unit is that object,
 * compiled into each of them. scene_e04 and scene_e05 link the same code,
 * but their seeds lie past the end of their extracted image, so they include
 * this file with ROOM_HOMING_PROJECTILE_EXTERNAL_SEEDS defined and name the
 * seeds in their symbol files. The launch parameters live in each room's
 * own data (g_RoomHomingLaunch).
 */
#include "pe1/room_homing_model.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte.h"

#ifndef ROOM_HOMING_PROJECTILE_EXTERNAL_SEEDS
GteRotation g_RoomHomingSparkRotation = { 0x400, 0, 0, 1 };
RenderColor g_RoomHomingSparkColor = { 0x50, 0x20, 0, 0 };
s16 g_RoomHomingReportFields[5] = { 0, 1, 2, 5, 6 };
GteRotation g_RoomHomingRingRotation = { 0x400, 0, 0, 1 };
RenderColor g_RoomHomingRingInner = { 0x20, 0x10, 8, 0 };
RenderColor g_RoomHomingRingOuter = { 0, 0, 0, 0 };
#endif

/* Spark left where the projectile bounces: lives eight frames, drawn as a
 * pulsing sprite in the current palette. */
int RoomEffect_HomingProjectileSpark(int mode, GteShortVector *spark) {
    int size;
    int kind;
    int palette;
    int frame;
    int pulse;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 9) {
            return 1;
        }
    } else if (mode == 2) {
        frame = D_800E27EC - 1;
        kind = D_800F336C;
        size = D_800966EC[((frame << 9) & 0x3E00) >> 2].word + 0x200;
        palette = D_800E1204[kind];
        /* The pulse reads the cosine half through its own copy of the
         * frame, as retail does. */
        pulse = frame;
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        clut = GetClut(0x80, palette);
        func_800CEE20(spark, &g_RoomHomingSparkRotation, (s16)size, (s16)size,
                      D_800F336A * 2 + 0xFD, clut, 1,
                      (s16)(D_800966EC[((pulse << 9) & 0x3E00) >> 2].word >> 16) >> 5,
                      &g_RoomHomingSparkColor);
    }
    return 0;
}

/* Thrown projectile: launched from the actor's hand matrix, it flies along
 * its heading, reflects off the walkable polygon's edges (one retry per
 * frame) or turns away from the player once outside, drops onto the floor
 * with a damped bounce, then reports its resting position through the
 * scene script.  Drawn as a glow sprite plus a floor ring while in flight. */
int RoomEffect_HomingProjectile(int mode) {
    RoomHomingModel *model = D_800E2368->model;
    GteVector position;
    s16 vector[4];
    s16 rotated[4];
    GteMatrix matrix;
    s32 area;
    RoomHomingModelActor *actor;
    int i;

    switch (mode) {
    case 0:
        model->state = 0;
        model->angle = D_800F32D0->actor->heading;
        return func_800CE560(D_800F33E0->pool, 8, 4,
                             RoomEffect_HomingProjectileSpark);
    case 1:
        actor = D_800F32D0->actor;
        switch (model->state) {
        case 0: {
            GteMatrix *transform;
            int frame = actor->frame;
            if (actor->phase == 8) {
                if (frame < 8) {
                    model->matrixIndex = 0x1A;
                } else {
                    model->matrixIndex = 9;
                }
            } else if (actor->phase == 9) {
                model->matrixIndex = 9;
                if (frame >= 2) {
                    model->state = 1;
                    model->soundHandle = func_8006DCE4(
                        0x585, actor->core->sound,
                        (s16)actor->matrices[9].t[0],
                        (s16)actor->matrices[9].t[1],
                        (s16)actor->matrices[9].t[2]);
                }
            }
            if (model->matrixIndex == 9) {
                vector[0] = -0x10;
                vector[1] = 0;
                vector[2] = -0x40;
            } else {
                vector[0] = 0;
                vector[1] = 0;
                vector[2] = 0;
            }
            transform = &actor->matrices[model->matrixIndex];
            gte_ldrotmatrix(transform);
            gte_ldtransmatrix(transform);
            gte_ldv0(vector);
            gte_rtv0tr_mac();
            gte_stsv(vector);
            for (i = 0; i < 3; i++) {
                model->position[i].word = vector[i] << 16;
            }
            return 0;
        }
        case 1: {
            s16 retries = 1;
            int point;
            int previous;
            int current;
            int sine;
            vector[0] = 0;
            vector[1] = 0;
            vector[2] = -model->speed >> 12;
            matrix.t[0] = 0;
            matrix.t[1] = 0;
            matrix.t[2] = 0;
            do {
                i = rcos(model->angle);
                sine = rsin(model->angle);
                matrix.m[0][0] = i;
                matrix.m[0][2] = sine;
                matrix.m[2][0] = -sine;
                matrix.m[2][2] = i;
                matrix.t[2] = 0;
                matrix.t[1] = 0;
                matrix.t[0] = 0;
                matrix.m[2][1] = 0;
                matrix.m[1][2] = 0;
                matrix.m[1][0] = 0;
                matrix.m[0][1] = 0;
                matrix.m[1][1] = 0x1000;
                gte_ldrotmatrix(&matrix);
                gte_ldtransmatrix(&matrix);
                gte_ldv0(vector);
                gte_rtv0tr_mac();
                gte_stsv(rotated);
                position.x = model->position[0].word + (rotated[0] << 12);
                position.z = model->position[2].word + (rotated[2] << 12);
                point = ((u32)position.x >> 16) | (position.z & 0xFFFF0000);
                previous = D_8009D248[D_8009D1CC - 1].x |
                           (D_8009D248[D_8009D1CC - 1].yz & 0xFFFF0000);
                i = 0;
                while (i < D_8009D1CC) {
                    current = D_8009D248[i].x | (D_8009D248[i].yz & 0xFFFF0000);
                    gte_ldsxy0(point);
                    gte_ldsxy2(previous);
                    gte_ldsxy1(current);
                    gte_nclip();
                    gte_stmac0(&area);
                    if (area < 0) break;
                    i++;
                    previous = current;
                }
                if (i >= D_8009D1CC) {
                    model->escaped = 1;
                    break;
                }
                if (model->escaped == 0) {
                    model->angle = (FieldEng_VecToAngle(&model->position[0].word,
                                                        D_8009D254->position) + 0x800) & 0xFFF;
                    break;
                }
                i = ratan2((s16)current - (s16)previous,
                              (current >> 16) - (previous >> 16)) & 0xFFF;
                if (i >= 0x800) i -= 0x800;
                retries--;
                model->angle = (i + (i - (u16)model->angle)) & 0xFFF;
            } while (retries != -1);
            model->position[0].word = position.x;
            model->position[2].word = position.z;
            if (retries <= 0) {
                model->speed = model->speed * model->damping / 16;
            }
            if (model->gravity != 0) {
                model->position[1].word -= model->lift;
                if ((model->position[1].word >> 16) >= model->floor - 0x20) {
                    model->position[1].word = (model->floor - 0x20) << 16;
                    model->lift = -model->lift * model->bounce / 16;
                    model->speed = model->speed * model->damping / 16;
                    if (model->lift <= 0x2FFFFF) {
                        model->gravity = 0;
                    } else {
                        RoomHomingModelSpark *spark = func_800CE610(D_800F33E0->pool);
                        if (spark != 0) {
                            spark->x = model->position[0].part.integer;
                            spark->y = D_800942EC.count;
                            spark->z = model->position[2].part.integer;
                        }
                    }
                } else {
                    model->lift -= model->gravity;
                }
            } else {
                model->speed -= model->drag;
                if (model->speed <= 0) model->finished = 1;
            }
            if (model->floor - 0x200 < model->position[1].part.integer) {
                area = func_800DFE20(&position.x, D_8009D254->position);
                if (area < 0x80) model->finished = 1;
            }
            if (model->finished == 0) return 0;
            if (model->soundHandle != -1) {
                func_800866A4(model->soundHandle, 0);
            }
            model->position[0].word = model->position[0].part.integer;
            model->position[1].word = model->position[1].part.integer + 0x20;
            model->position[2].word = model->position[2].part.integer;
            {
                int handle = func_8006F39C(0x25, actor);
                u32 n = 0;
                do {
                    func_8006F6D4(handle, 0, n + 1,
                                  ((s32 *)model)[g_RoomHomingReportFields[n]],
                                  0, 0);
                    n++;
                } while (n < 5);
            }
            return 1;
        }
        }
        return 1;
    case 2: {
        GteShortVector sprite;
        int axis;
        int frame;
        int scale;
        int intensity;
        u16 clut;
        int kind;
        int palette;
        u16 *tpages;
        for (axis = 0; axis < 3; axis++) {
            ((s16 *)&sprite)[axis] = model->position[axis].part.integer;
        }
        frame = D_800E27EC;
        if (frame < 8) {
            scale = D_800966EC[((frame << 10) & 0x3C00) >> 2].part.sin + 0x1000;
            intensity = 0x80;
        } else {
            intensity = D_800966EC[((frame << 12) & 0x3000) >> 2].part.cos / 64 + 0x60;
            scale = 0x1000;
        }
        D_800F3368.parameter00 = 0x20;
        {
            int index = D_800E11EC.index;
            int tpage;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            tpage = D_800E2850[index];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            D_800F3372.parameter0A = 0;
            D_800F3372.depth = 0;
            D_800F3368.tpage = tpage;
        }
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        tpages = D_800E2850;
        clut = func_80077AA4(0x30, palette);
        func_800CEE20(&sprite, 0, scale, scale, 0x42, clut, 1, intensity, 0);
        if (model->state != 0) {
            sprite.y = model->floor;
            func_800D004C(&sprite, 0x50, 0x50, 8,
                          &g_RoomHomingRingRotation, 0x1000, 0x1000,
                          &g_RoomHomingRingInner,
                          &g_RoomHomingRingOuter, intensity, 1);
        }
        D_800F3368.parameter00 = 0x10;
        {
            int index = D_800E11E8.index;
            int tpage;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            tpage = tpages[index];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    }
    return 0;
}

/* Throw controller: seeds the model from the launch parameters, then
 * follows the actor's throw animation until it either releases the
 * projectile (the cue reaches the last frame) or is interrupted. */
int RoomEffect_HomingProjectileThrow(int mode, RoomHomingModel *model) {
    RoomHomingModelActor *actor = D_800F32D0->actor;
    u8 *cue;
    int motion;
    u16 frame;
    u16 lastFrame;

    if (mode == 0) {
        int floor;
        D_800E2368->model = model;
        actor->core->flags |= 0x40000000;
        floor = D_800942EC.height;
        model->matrixIndex = 0x1A;
        model->finished = 0;
        model->escaped = 0;
        model->released = 0;
        model->armed = 0;
        model->soundHandle = -1;
        model->floor = floor;
        model->speed = g_RoomHomingLaunch.speed;
        model->drag = g_RoomHomingLaunch.drag;
        model->lift = g_RoomHomingLaunch.lift;
        model->gravity = g_RoomHomingLaunch.gravity;
        model->scriptArg0 = g_RoomHomingLaunch.scriptArg0;
        model->scriptArg1 = g_RoomHomingLaunch.scriptArg1;
        model->bounce = g_RoomHomingLaunch.bounce;
        model->damping = g_RoomHomingLaunch.damping;
    } else if (mode == 1) {
        if (model->released == 0) {
            if (actor->core == 0 || func_8003010C(actor, 0x2C) <= 0) {
                model->released = 1;
            } else if ((actor->core->flags & 0x1800) != 0) {
                model->released = 1;
            } else if ((int)((actor->core->flags >> 1) & 7) > 0) {
                model->released = 1;
            }
        }
        if (model->state == 0 && model->armed != 0 && *actor->core->cue == 0) {
            model->released = 1;
        }
        if (model->released != 0) {
            if (model->state != 0) {
                return model->finished != 0;
            }
            model->state = -1;
            return 1;
        }

        cue = actor->core->cue;
        if (*cue == 1) {
            *cue = 2;
            model->armed = 1;
        }
        if (model->finished != 0) {
            model->finished++;
        }
        if (actor->phase != 9) {
            return 0;
        }
        /* Two reads of the frame, as retail: the last-frame test keeps
         * its own copy. */
        frame = actor->frame;
        lastFrame = actor->frame;
        motion = actor->motion;
        if (motion >= 0) {
            if ((s16)frame >= 0x1C && model->finished < 0x18) {
                actor->motion = -motion;
                return 0;
            }
            if ((s16)lastFrame < actor->frameCount - 1) {
                return 0;
            }
            cue = actor->core->cue;
            if (*cue == 2) {
                *cue = 4;
            }
            return 1;
        }
        if ((s16)frame < 0x17) {
            actor->motion = -motion;
        }
    }
    return 0;
}

/* Scene script entry: mode 0 sets speed, drag and damping, mode 1 lift,
 * gravity and bounce, mode 2 the two values reported back. */
int RoomEffect_HomingProjectileConfigure(u32 mode, int a, int b, int c) {
    switch (mode) {
    case 0:
        g_RoomHomingLaunch.speed = a;
        g_RoomHomingLaunch.drag = b;
        g_RoomHomingLaunch.damping = c;
        if (c == 0) {
            g_RoomHomingLaunch.damping = 8;
        }
        break;
    case 1:
        g_RoomHomingLaunch.lift = a;
        g_RoomHomingLaunch.gravity = b;
        g_RoomHomingLaunch.bounce = c;
        if (c == 0) {
            g_RoomHomingLaunch.bounce = 10;
        }
        break;
    case 2:
        g_RoomHomingLaunch.scriptArg0 = a;
        g_RoomHomingLaunch.scriptArg1 = b;
        break;
    }
    return 0;
}
