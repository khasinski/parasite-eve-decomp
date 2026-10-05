#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_m086_seeker.h"

/* Seeker controller: anchored on joint 7 of the actor, it drifts and falls
 * (shedding sparks on the first frame), then bursts into a spark ring and a
 * fan of seekers once it reaches the floor band. */
int func_8018F734(int mode, RoomM086SeekerController *state, RoomM086ControllerParams *params) {
    GteShortVector rotation;
    GteShortVector position;
    RoomM086Offset anchor = D_8018EFF4;
    RoomM086Offset drift = D_8018EFFC;
    GteMatrix matrix;
    GteVector scale;
    RoomM086MatrixSlot *slot;
    RoomM086Seeker *child;
    u16 *index;
    int size;
    int page;
    int speed;
    int angle;
    int i;

    switch (mode) {
    case 0:
        slot = &D_800BCFA4;
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        func_800CE8F0(D_800F32D0->pool, 7, &anchor, state);
        func_800CEAE8(D_800F32D0->pool->bone, &drift, &state->velocity);
        state->state = 0;
        state->timer = 0;
        state->velocity.y -= 0x18;
        func_800CE9D4(D_800F32D0->pool, 0, &state->rotation);
        if (D_800E2368->active) {
            RoomM086Pool *pool = D_800F32D0->pool;
            if (pool && pool->object) {
                u8 *flag = pool->object->state;
                if (*flag == 1) *flag = 2;
            }
        }
        D_80190B88 = func_8006E498(D_800B0E64.base, 0xC5465704);
        func_800C6D5C(D_80190B88, 0, 0);
        D_80190B84 = func_8006E498(D_800B0E64.base, 0xC5865704);
        func_800C6D5C(D_80190B84, 0, 0);
        func_800D3F64(0x577, func_800D3FD8());
        return func_800CE560(D_800F33E0->pool, 0x14, 0xE, func_8018F004);
    case 1:
        switch (state->state) {
        case 0:
            state->timer++;
            state->position.x += state->velocity.x;
            state->position.y += state->velocity.y;
            state->position.z += state->velocity.z;
            state->velocity.x = state->velocity.x * 31 / 32;
            state->velocity.z = state->velocity.z * 31 / 32;
            state->velocity.y += 5;
            if (D_800E27EC == 1) {
                for (i = 0; i < 6; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->position.x = state->position.x;
                        child->position.y = state->position.y;
                        child->position.z = state->position.z;
                        child->position.x += (func_80071A54() & 0x7F) - 0x40;
                        child->position.y += (func_80071A54() & 0x7F) - 0x40;
                        child->position.z += (func_80071A54() & 0x7F) - 0x40;
                        child->state = 1;
                        child->timer = 0;
                    }
                }
            }
            if (state->position.y < D_800942EC.count - 0x202) return 0;
            angle = func_80071A54();
            for (i = 0; i < 6; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->position.x = state->position.x;
                    child->position.y = state->position.y;
                    child->position.z = state->position.z;
                    speed = (func_80071A54() & 0x1F) + 0x10;
                    child->heading.x = func_80077DC4(angle) * speed / 4096;
                    child->heading.z = func_80077CF4(angle) * speed / 4096;
                    child->heading.y = -8 - (func_80071A54() & 0xF);
                    child->state = 2;
                    child->timer = 0;
                }
                angle += 0x2AA;
            }
            angle = state->rotation.y - (params->spacing * params->count) / 2;
            for (i = 0; i < params->count; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->position.x = state->position.x;
                    child->position.y = state->position.y;
                    child->position.z = state->position.z;
                    child->heading.x = state->rotation.x;
                    child->heading.y = state->rotation.y;
                    child->heading.z = state->rotation.z;
                    child->heading.x -= 0xC8;
                    child->heading.y = angle;
                    child->state = 0;
                    child->timer = 0;
                }
                angle += params->spacing;
            }
            state->state = 1;
            state->timer = 0;
            break;
        case 1:
            if (++state->timer >= 8) {
                state->state = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->timer++;
            if (state->timer >= 0x40) return 1;
            break;
        }
        break;
    case 2:
        D_800F3368.depth = 4;
        D_800F3368.parameter0A = 0;
        size = params->size / 20;
        switch (state->state) {
        case 0:
            rotation.x = state->rotation.x;
            rotation.y = state->rotation.y;
            rotation.z = state->rotation.z;
            rotation.z += state->timer * 128;
            index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 0xFF, 0, 0));
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                GsSetOrign(page, func_80077AA4(0x10, palette));
            }
            func_800C6ED8(0);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = state->position.x;
            matrix.t[1] = state->position.y;
            matrix.t[2] = state->position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            Gte_ScaleMatrix(&matrix, &scale);
            func_800C6EF8(D_80190B88);
            func_800C6FA0(D_80190B88, 0x80);
            func_800C71E4(D_80190B88, &matrix);
            func_800C6F4C(D_80190B88);
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = 0;
            rotation.pad = 1;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            position.y = D_800942EC.count;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, &rotation, size * 20, size * 20,
                              (s16)D_800F3368.parameter02 + 0xFD,
                              func_80077AA4(0x80, palette), 2, 0x80, 0);
            }
            break;
        case 1:
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&state->position, 0, size * 20, size * 20,
                              (s16)D_800F3368.parameter02 * state->timer + 0x20,
                              func_80077AA4(0x80, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            break;
        }
        break;
    }
    return 0;
}
