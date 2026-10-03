#include "pe1/room_homing_model.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_HOMING_PROJECTILE_FUNC
#error "ROOMEFFECT_HOMING_PROJECTILE_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_HOMING_PROJECTILE_CALLBACK
#error "ROOMEFFECT_HOMING_PROJECTILE_CALLBACK must name the spark callback"
#endif
#ifndef ROOMEFFECT_HOMING_PROJECTILE_FIELDS
#error "ROOMEFFECT_HOMING_PROJECTILE_FIELDS must name the script field table"
#endif
#ifndef ROOMEFFECT_HOMING_PROJECTILE_ROTATION
#error "ROOMEFFECT_HOMING_PROJECTILE_ROTATION must name the ring rotation"
#endif
#ifndef ROOMEFFECT_HOMING_PROJECTILE_COLOR0
#error "ROOMEFFECT_HOMING_PROJECTILE_COLOR0 must name the ring inner colour"
#endif
#ifndef ROOMEFFECT_HOMING_PROJECTILE_COLOR1
#error "ROOMEFFECT_HOMING_PROJECTILE_COLOR1 must name the ring outer colour"
#endif

extern int ROOMEFFECT_HOMING_PROJECTILE_CALLBACK(int mode, GteShortVector *spark);
extern s16 ROOMEFFECT_HOMING_PROJECTILE_FIELDS[];
extern GteRotation ROOMEFFECT_HOMING_PROJECTILE_ROTATION;
extern RenderColor ROOMEFFECT_HOMING_PROJECTILE_COLOR0;
extern RenderColor ROOMEFFECT_HOMING_PROJECTILE_COLOR1;

/* Thrown projectile: launched from the actor's hand matrix, it flies along
 * its heading, reflects off the walkable polygon's edges (one retry per
 * frame) or turns away from the player once outside, drops onto the floor
 * with a damped bounce, then reports its resting position through the
 * scene script.  Drawn as a glow sprite plus a floor ring while in flight. */
int ROOMEFFECT_HOMING_PROJECTILE_FUNC(int mode) {
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
                             ROOMEFFECT_HOMING_PROJECTILE_CALLBACK);
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
                i = Gte_Atan2((s16)current - (s16)previous,
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
                                  ((s32 *)model)[ROOMEFFECT_HOMING_PROJECTILE_FIELDS[n]],
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
            scale = D_800966EC[((frame << 10) & 0x3C00) / 2] + 0x1000;
            intensity = 0x80;
        } else {
            intensity = D_800966EE[((frame << 12) & 0x3000) / 2] / 64 + 0x60;
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
                          &ROOMEFFECT_HOMING_PROJECTILE_ROTATION, 0x1000, 0x1000,
                          &ROOMEFFECT_HOMING_PROJECTILE_COLOR0,
                          &ROOMEFFECT_HOMING_PROJECTILE_COLOR1, intensity, 1);
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

#undef ROOMEFFECT_HOMING_PROJECTILE_FUNC
#undef ROOMEFFECT_HOMING_PROJECTILE_CALLBACK
#undef ROOMEFFECT_HOMING_PROJECTILE_FIELDS
#undef ROOMEFFECT_HOMING_PROJECTILE_ROTATION
#undef ROOMEFFECT_HOMING_PROJECTILE_COLOR0
#undef ROOMEFFECT_HOMING_PROJECTILE_COLOR1
