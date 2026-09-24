/* Scene e01 motion update, retail offset 0x350. */
#include "common.h"
#include "pe1/room_fx.h"
#include "pe1/room_motion_trigger.h"

extern FieldActor *volatile D_8009D254;
extern RoomFxVec4 D_8018EFF4;

s32 func_80077DC4(s32 arg0);
s32 func_80077CF4(s32 arg0);
void func_80078CC4(void *matrix, RoomFxVec4 *scale);
s32 func_8003010C(void *arg0, s32 arg1);
void func_80020CE4(void);
void func_8018F5D4(void *arg0, void *arg1);
void func_8018F6BC(void *arg0);

void func_8018F338(RoomMotionTrigger *arg0) {
    FieldActor *s1 = arg0->probe_actor;
    /* PIN-DEBT: retail holds the matrix base (D_8009D254) in $v1 for the field
     * stores; GCC otherwise picks $a0. */
    register FieldActor *g asm("$3");
    RoomFxVec4 scale;
    s32 dc4, cf4;
    /* PIN-DEBT: retail stores -cf4 from $a0 and emits the 0x1EA store first. */
    register s32 neg asm("$4");
    s32 s0;
    /* PIN-DEBT: retail keeps a second live copy of the angle in $s4; without the
     * pin GCC copy-propagates s4=s0 away and drops the callee-saved slot. */
    register s32 s4 asm("$20");
    s32 kind;
    s32 val;
    /* PIN-DEBT: preserve the raw 16-bit angle in $v1 across both comparisons. */
    register s32 raw_val asm("$3");
    /* PIN-DEBT: the sign-extended comparisons use $v0 in retail. */
    register s32 signed_val asm("$2");
    register s32 signed_s4 asm("$2");
    FieldActor *matrix_arg;
    s32 *p;
    void *s3v = &arg0->callback;

    dc4 = func_80077DC4(0xC00);
    cf4 = func_80077CF4(0xC00);
    g = D_8009D254;
    neg = -cf4;
    *(volatile s16 *)&g->render_object.model_matrix.rotation[0][1] = neg;
    matrix_arg = D_8009D254;
    *(volatile s16 *)&g->render_object.model_matrix.rotation[1][0] = cf4;
    g->render_object.model_matrix.rotation[0][0] = dc4;
    g->render_object.model_matrix.rotation[1][1] = dc4;
    g->render_object.model_matrix.translation[2] = 0;
    g->render_object.model_matrix.translation[1] = 0;
    g->render_object.model_matrix.translation[0] = 0;
    g->render_object.model_matrix.rotation[2][1] = 0;
    g->render_object.model_matrix.rotation[2][0] = 0;
    g->render_object.model_matrix.rotation[1][2] = 0;
    g->render_object.model_matrix.rotation[0][2] = 0;
    g->render_object.model_matrix.rotation[2][2] = 0x1000;
    scale = D_8018EFF4;
    func_80078CC4(&matrix_arg->render_object.model_matrix, &scale);
    g = D_8009D254;
    g->render_object.model_matrix.translation[0] = 0x100;
    g->render_object.model_matrix.translation[1] = -0x20;
    g->render_object.model_matrix.translation[2] = -0xC0;
    s0 = s1->anim.parts.integer;
    g->pos_x = arg0->saved_x;
    g->pos_z = arg0->saved_z;
    s4 = s0;

    if (s1->state != 0) {
        if (func_8003010C(s1, 0x2C) > 0) {
            goto L478;
        }
    }
    if (D_8009D254->state->amount <= 0) {
        goto L59C;
    }
    goto L594;

L478:
    if (D_8009D254->state->amount <= 0) {
        goto L59C;
    }
    kind = s1->mode;
    if (kind == 7) {
        goto L4F8;
    }
    if (kind >= 8) {
        goto L4C4;
    }
    if (kind == 6) {
        goto L5B0;
    }
    goto L594;

L4C4:
    if (kind != 0x10) {
        goto L594;
    }
    if ((s16)s0 < 0x12) {
        goto L5B0;
    }
    p = arg0->completion_state;
    if (p != 0) {
        *p = 2;
    }
    goto L5B0;

L4F8:
    val = ((u16 *)&s1->anim_prev)[1];
    /* Empty barriers retain the raw load and the two separate sign extensions. */
    asm volatile("" : "=r"(val) : "0"(val));
    raw_val = val;
    asm volatile("" : "=r"(raw_val) : "0"(raw_val));
    if ((s16)val < 0xE) {
        if ((s16)s0 >= 0xE) {
            goto L594;
        }
    }
    signed_val = (s16)raw_val;
    asm volatile("" : "=r"(signed_val) : "0"(signed_val));
    if (signed_val >= 3) {
        goto L5B0;
    }
    signed_s4 = (s16)s4;
    asm volatile("" : "=r"(signed_s4) : "0"(signed_s4));
    if (signed_s4 < 3) {
        goto L5B0;
    }
    D_8009D254->state->flags |= 0x4000;
    if (s1->state != 0) {
        s1->state->core_flags |= 0x80000000;
    }
    goto L5B0;

L594:
    func_80020CE4();
L59C:
    func_8018F5D4(s1, s3v);
    func_8018F6BC(arg0);
L5B0:
    ;
}
