/* room_m269, file offset 0x1A4, 428 retail-matching bytes. */
#include "common.h"
#include "pe1/field_actor.h"

typedef struct { s32 x, y, z; } Vec3Fixed;

typedef struct RoomMotionTrigger {
    u8 reserved00[8];
    FieldActor *probe_actor;
    void (*callback)(void);
    u8 reserved10[0xC];
    FieldActor *source_actor;
    s32 saved_x;
    s32 saved_z;
    u8 activated;
} RoomMotionTrigger;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, probe_actor) == 0x08,
                  room_motion_trigger_probe_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, source_actor) == 0x1C,
                  room_motion_trigger_source_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomMotionTrigger, activated) == 0x28,
                  room_motion_trigger_active_offset);

extern FieldActor *D_8009D254;

void func_8003E0FC(void *dst, s32 count, Vec3Fixed *src);
s32 func_800DFE20(Vec3Fixed *lhs, Vec3Fixed *rhs);
void func_80020C74(void);
s32 func_80077DC4(s32 arg0);
s32 func_80077CF4(s32 arg0);
void func_8018F6BC(void *arg);
void func_8018F338(void);

void func_8018F18C(RoomMotionTrigger *arg) {
    u8 mode = D_8009D254->mode;
    s32 kind = arg->probe_actor->anim.parts.integer;

    if (mode >= 0x12) {
        func_8018F6BC(arg);
        return;
    }
    if (kind < 8) {
        return;
    }
    if (kind < 0xF) {
        Vec3Fixed *sp = (Vec3Fixed *)0x1F800008;
        s32 v;
        s32 s;
        /* Retain the negated sine in the call-argument register. */
        register s32 neg asm("$4");
        FieldActor *p;
        FieldActor *actor;

        func_8003E0FC(&arg->source_actor->render_object, 6, sp);
        sp->x <<= 16;
        sp->z <<= 16;
        if (func_800DFE20((Vec3Fixed *)&D_8009D254->pos_x, sp) >= 0x100) {
            return;
        }
        arg->callback = func_8018F338;
        func_80020C74();

        actor = D_8009D254;
        actor->render_object.animation_source = &arg->source_actor->render_object;
        actor->render_object.animation_state = 4;
        actor->render_object.animation_id = 6;
        actor->flags |= 0x10000;
        actor->render_object.flags_9C |= 0x400;
        s = func_80077DC4(0xC00);
        v = func_80077CF4(0xC00);
        neg = -v;

        p = D_8009D254;
        p->render_object.model_matrix.rotation[1][0] = v;
        v = 0x1000;
        p->render_object.model_matrix.rotation[2][2] = v;
        *(volatile s32 *)&p->render_object.model_matrix.translation[0] = 0;
        v = 0x100;
        *(volatile s32 *)&p->render_object.model_matrix.translation[0] = v;
        *(volatile s32 *)&p->render_object.model_matrix.translation[2] = 0;
        v = -0xC0;
        *(volatile s32 *)&p->render_object.model_matrix.translation[2] = v;
        p->render_object.model_matrix.rotation[0][0] = s;
        p->render_object.model_matrix.rotation[0][1] = neg;
        p->render_object.model_matrix.rotation[1][1] = s;
        *(volatile s32 *)&p->render_object.model_matrix.translation[1] = 0;
        p->render_object.model_matrix.rotation[2][1] = 0;
        p->render_object.model_matrix.rotation[2][0] = 0;
        p->render_object.model_matrix.rotation[1][2] = 0;
        p->render_object.model_matrix.rotation[0][2] = 0;
        *(volatile s32 *)&p->render_object.model_matrix.translation[1] = 0;
        arg->activated = 1;

        p = D_8009D254;
        arg->saved_x = p->pos_x;
        arg->saved_z = p->pos_z;
        return;
    }
    func_8018F6BC(arg);
}
