#include "common.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_anim.h"
#include "pe1/field_engine_slot.h"

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1,
                                   FieldAnimPointData *points) {
    volatile int stack_pad;
    FieldBillboard *out;
    unsigned int i;

    i = 0;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    D_800E2818.billboard.brightness = (points->scale << 24) >> 23;

    if (points->count != 0) {
        out = &D_800E2818.billboard;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                out->position.x = points->x[i];
                out->position.y = points->y[i];
                out->position.z = points->z[i];
                func_800C3B04(out);
                i++;
            } while (i < points->count);
        } while (0);
    }
}

#include "common.h"


int func_800CC878(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = anim->point.x;
    output = &D_800F3338.oriented;
    output->position.x = value_v0;
    D_800F3338.oriented.position.y = anim->point.y;
    D_800F3338.oriented.position.z = anim->point.z;
    D_800F3338.oriented.scale.x = anim->extent;
    D_800F3338.oriented.scale.y = anim->extent;
    D_800F3338.oriented.scale.z = anim->extent;
    D_800F3338.oriented.brightness = (signed char)anim->scale;
    func_800C3324(output);
}

#include "common.h"
int func_800CC92C(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    u16 value;
    register int frame asm("$3");

    value = anim->point.x;
    frame = anim->scale;
    anim->point.x = value;
    value = anim->point.y;
    frame -= 2;
    anim->scale = frame;
    value -= 0xA;
    anim->point.y = value;
    value = (u16)anim->extent;
    frame = (s8)anim->scale;
    frame = frame < 0x1E;
    value += 0x1E;
    anim->extent = value;
    if (frame) {
        state->flag = 2;
    }
}

#include "common.h"
int func_800CC974(void *arg0, FieldEngSlot *state, FieldAnimRadialParticles *anim) {
    int i;
    FieldEngSlot *state_t2;
    register int dx asm("$3");
    int dy;
    register int dz asm("$5");
    int frame;

        state_t2 = state;
    asm volatile("" : "=r"(state_t2) : "0"(state_t2));
    i = 0;
    if ((signed char)anim->count > 0) {
        do {
            register int pos asm("$2");
            register int angle asm("$3");

            dx = anim->velocity[i].x >> 9;
            pos = anim->points[i].position.x;
            dy = anim->velocity[i].y >> 9;
            dz = anim->velocity[i].z >> 9;
            pos += dx;
            anim->points[i].position.x = pos;
            pos = anim->points[i].position.y;
            angle = (u16)anim->velocity[i].y;
            pos += dy;
            anim->points[i].position.y = pos;
            pos = anim->points[i].position.z;
            angle += 0xB4;
            anim->velocity[i].y = angle;
            pos += dz;
            anim->points[i].position.z = pos;
            pos = anim->extent[i];
            pos += 0x18;
            anim->extent[i] = pos;
            i++;
        } while (i < anim->count);
    }

    anim->scale -= 2;
    if (anim->scale < 2) {
        state_t2->flag = 2;
    }
    asm volatile("" : : "r"(&frame));
}

#include "common.h"
int func_800CCA40(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 2;
    anim->extent = (u16)anim->extent + 0x28;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

#include "common.h"
int func_800CCA78(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 6;
    anim->extent = (u16)anim->extent + 0xB4;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

#include "common.h"
int func_800CCAB0(void *arg0, FieldEngSlot *state, FieldAnimScatteredParticles *anim) {
    int i;
    FieldEngSlot *state_t1;
    register int dx asm("$3");
    int dy;
    register int dz asm("$5");
    int frame;

    state_t1 = state;
    asm volatile("" : "=r"(state_t1) : "0"(state_t1));
    i = 0;
    if (anim->points.count > 0) {
        do {
            register int pos asm("$2");
            register int verticalVelocity asm("$3");

            dx = anim->velocity_x[i] >> 8;
            pos = anim->points.x[i];
            dy = anim->velocity_y[i] >> 8;
            dz = anim->velocity_z[i] >> 8;
            pos += dx;
            anim->points.x[i] = pos;
            pos = anim->points.y[i];
            verticalVelocity = (u16)anim->velocity_y[i];
            pos += dy;
            anim->points.y[i] = pos;
            pos = anim->points.z[i];
            verticalVelocity += 0xB4;
            anim->velocity_y[i] = verticalVelocity;
            pos += dz;
            anim->points.z[i] = pos;
            i++;
        } while (i < anim->points.count);
    }

    anim->points.scale -= 2;
    if ((signed char)anim->points.scale < 2) {
        state_t1->flag = 2;
    }
    asm volatile("" : : "r"(&frame));
}

#include "common.h"
int func_800CCB6C(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 8;
    anim->extent = (u16)anim->extent + 0x1A4;
    if ((s8)anim->scale < 0x14) {
        anim->scale = 0;
        state->flag = 2;
    }
}
