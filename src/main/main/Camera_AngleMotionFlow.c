#include "common.h"
#include "pe1/global_slot.h"
#include "pe1/task_node.h"

PE1_STATIC_ASSERT(sizeof(TaskNode) == 0x2C, camera_task_node_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, flags) == 0x08,
                  camera_task_node_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, active) == 0x10,
                  camera_task_node_active_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, target14) == 0x14,
                  camera_task_node_target14_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, target18) == 0x18,
                  camera_task_node_target18_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, target1c) == 0x1C,
                  camera_task_node_target1c_offset);
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))

extern Pe1GlobalSlot D_8009D254;
extern u8 *D_8009D2F0[];
extern TaskNode *D_8009D300;
extern int *D_8009CE00;

int Math_FixedMul(int a, int b);
int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);

int Camera_TrackRelativeOffset(int **args) {
    u8 *speed_entity;
    u8 *motion_entity;
    TaskNode *init_state;
    TaskNode *camera_state;
    int movement_speed;
    int turn_speed;
    int target_angle;
    int desired_angle;
    int current_angle;
    int angle_difference;
    int wrap_difference;
    int current_x;
    int current_z;
    int target_x;
    int target_z;
    int movement_z;
    int remaining_distance;
    int movement_distance;

    speed_entity = D_8009D2F0[0];
    current_x = S32_AT(speed_entity, 0x28);
    current_z = S32_AT(speed_entity, 0x30);
    if (speed_entity != D_8009D254.value.pointer) {
        movement_speed = S32_AT(speed_entity, 0x20);
    } else {
        movement_speed = Math_FixedMul(0x50000, S32_AT(speed_entity, 0x20));
    }
    movement_speed = Math_FixedMul(movement_speed, U16_AT(D_8009D2F0[0], 0x26) << 4);

    init_state = D_8009D300;
    if ((init_state->flags & 0x20) == 0) {
        turn_speed = *args[2];
        target_x = current_x + *args[0];
        target_z = current_z + *args[1];
        init_state->flags |= 0x20;
        S32_AT(init_state, 0x20) = 0;
        init_state->target14 = turn_speed;
        init_state->target1c = target_z;
        init_state->target18.coordinate = target_x;
    } else {
        turn_speed = init_state->target14;
        target_x = init_state->target18.coordinate;
        target_z = init_state->target1c;
    }

    target_angle = (u16)(0x1400 - Gte_Atan2(current_z - target_z,
                                             current_x - target_x));
    target_angle &= 0xFFF;
    desired_angle = target_angle;
    if (turn_speed != 0) {
        current_angle = S16_AT(D_8009D2F0[0], 0x3A);
        if (current_angle < desired_angle) {
            angle_difference = desired_angle - current_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    target_angle = desired_angle;
                } else {
                    target_angle = current_angle + turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                target_angle = desired_angle;
            } else {
                target_angle = current_angle - turn_speed;
                if (target_angle < 0) {
                    wrap_difference = current_angle + 0x1000;
                    wrap_difference -= desired_angle;
                    if (wrap_difference < turn_speed) {
                        target_angle = desired_angle;
                    }
                }
            }
        } else {
            angle_difference = current_angle - desired_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    target_angle = desired_angle;
                } else {
                    target_angle = current_angle - turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                target_angle = desired_angle;
            } else {
                target_angle = current_angle + turn_speed;
                if (target_angle >= 0x1001) {
                    wrap_difference = desired_angle + 0x1000;
                    wrap_difference -= current_angle;
                    if (wrap_difference < turn_speed) {
                        target_angle = desired_angle;
                    }
                }
            }
        }
        target_angle &= 0xFFF;
        S16_AT(D_8009D2F0[0], 0x3A) = target_angle;
    }

    S32_AT(D_8009D2F0[0], 0x68) = Math_FixedMul(-movement_speed, rsin(target_angle) << 4);
    movement_z = Math_FixedMul(-movement_speed, rcos(target_angle) << 4);
    current_x = (target_x - current_x) >> 16;
    current_z = (target_z - current_z) >> 16;
    remaining_distance = current_x * current_x + current_z * current_z;
    motion_entity = D_8009D2F0[0];
    current_x = S16_AT(motion_entity, 0x6A);
    current_z = movement_z >> 16;
    movement_distance = current_x * current_x + current_z * current_z;
    S32_AT(motion_entity, 0x70) = movement_z;
    if (movement_distance >= remaining_distance) {
        camera_state = D_8009D300;
        S32_AT(motion_entity, 0x28) = target_x;
        S32_AT(motion_entity, 0x30) = target_z;
        S32_AT(motion_entity, 0x68) = 0;
        S32_AT(motion_entity, 0x70) = 0;
        camera_state->flags &= 0xFFDF;
        return 1;
    } else {
        D_8009CE00 -= 5;
        D_8009D300->active = 1;
        return 0;
    }
}
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern s32 g_SceneDataTable0;
extern void * g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])
extern TaskNode *g_TaskNodePool;

s32 Camera_StepAngleFade(u8 *arg0) {
    register s32 temp_a1 asm("$5");
    s32 ta1c;
    register s32 ta1b asm("$6");
    s32 temp_t1;
    TaskNode *p300;
    register s32 temp_v1 asm("$8");
    s32 tv1a;
    u16 temp_a3;
    register s32 tv0c asm("$8");
    register s32 tv0r asm("$2");
    register s32 ta12c asm("$7");
    register s32 ta1d asm("$4");
    s32 temp_a1_2;
    s32 sv0;
    register s32 temp_v1x asm("$3");
    s32 var_v1;
    register TaskNode *p13 asm("$2");
    register s32 ret asm("$2");
    s32 tv0;
    register u32 t3araw asm("$2");
    s32 *tv0p;
    register u16 *pa3 asm("$3");
    s32 tor;
    register s32 tdiff asm("$2");
    s32 vext;
    s32 vext2;
    s32 sa3a;
    s32 sa3b;
    s32 sa3c;
    register s32 sv1f asm("$2");
    s32 temp_v0_2;
    s32 tve;
    TaskNode *p24;
    u8 *p26;
    s32 t3aa;
    register s32 sumb asm("$2");
    s32 tmask4;
    register s32 ta12d asm("$7");
    s32 sa12a;
    s32 sa12b;
    s32 temp_v1_2;
    s32 temp_v0_3;
    s32 temp_v0_4;

    t3araw = M2C_FIELD(g_CurrentEntity, u16 *, 0x3A);
    temp_a1 = t3araw & 0xFFF;
    p300 = g_TaskNodePool;
    temp_t1 = p300->flags;
    ta1c = temp_a1;
    if (!(temp_t1 & 0x20)) {
        ta1b = temp_a1;
                tv0p = M2C_FIELD(arg0, s32 **, 0);
        pa3 = M2C_FIELD(arg0, u16 **, 4);
        tv0 = *tv0p;
        temp_a3 = *pa3;
        temp_v1x = tv0 & 0xFFF;
        tv1a = temp_v1x;
        __asm__("" : "=r"(tv1a) : "0"(tv1a));
        temp_v1 = temp_v1x;
        if (ta1b == tv1a) {
            ret = 1;
            goto exit;
        }
        tor = temp_t1 | 0x20;
        p300->flags = (u16) tor;
                tdiff = tv1a - ta1b;
        if (tdiff >= 0) {
            var_v1 = temp_v1x - temp_a1;
        } else {
            var_v1 = temp_a1 - temp_v1x;
        }
        if ((s16) temp_v1 < ta1c) {
            vext = var_v1 << 0x10;
            if ((vext >> 0x10) >= 0x801) {
                sa3a = (s16) temp_a3;
                g_TaskNodePool->target14 = sa3a;
            } else {
                goto block_11;
            }
        } else if ((vext2 = var_v1 << 0x10), ((vext2 >> 0x10) < 0x801)) {
            sa3b = (s16) temp_a3;
            g_TaskNodePool->target14 = sa3b;
        } else {
block_11:
            sa3c = -(s16) temp_a3;
            g_TaskNodePool->target14 = sa3c;
        }
        sv1f = (s16) temp_v1;
        g_TaskNodePool->target18.coordinate = sv1f;
    }
    p13 = g_TaskNodePool;
    ta1d = ta1c;
    temp_a1_2 = p13->target14;
    tv0r = p13->target18.coordinate;
    __asm__("" : "=r"(tv0r) : "0"(tv0r));
    tv0c = tv0r;
    __asm__("" : "=r"(tv0c) : "0"(tv0c));
    sv0 = (s16) tv0r;
    ta12c = temp_a1_2;
    if (ta1d < sv0) goto chk2;
    sa12a = (s16) temp_a1_2;
    if (sv0 < (ta1d + sa12a)) goto chk2;
    goto block_24;
chk2:
    ta12d = ta12c;
    __asm__("" : "=r"(ta12d) : "0"(ta12d));
    if (sv0 < ta1d) {
        tve = ta12c << 0x10;
        goto inner;
    }
    sa12b = temp_a1_2 << 0x10;
    sumb = ta1d + (sa12b >> 0x10);
        if (sumb < sv0) {
        tve = ta12d << 0x10;
        goto inner;
    }
    goto block_24;
inner:
    {
        temp_v0_2 = tve >> 0x10;
        temp_v1_2 = ta1c + temp_v0_2;
        if (temp_v0_2 > 0) {
            if (temp_v1_2 >= 0x1001) {
                if ((temp_v1_2 & 0xFFF) < (s16) tv0c) {
                    goto block_21;
                }
                goto block_24;
            }
            goto block_21;
        }
block_21:
        temp_v0_3 = (s16) ta12c;
        if (temp_v0_3 < 0) {
            temp_v0_4 = ta1c + temp_v0_3;
            if (temp_v0_4 < 0) {
                tmask4 = temp_v0_4 & 0xFFF;
                if ((s16) tv0c < tmask4) {
                    goto block_26;
                }
                goto block_24;
            }
        }
        goto block_26;
    }
block_24:
    p24 = g_TaskNodePool;
    M2C_FIELD(g_CurrentEntity, u16 *, 0x3A) = (u16) tv0c;
    p24->flags = (u16) (p24->flags & 0xFFDF);
    ret = 1;
    goto exit;
block_26:
    ret = 0;
        p26 = g_CurrentEntity;
    t3aa = M2C_FIELD(p26, u16 *, 0x3A);
    g_SceneDataTable0 -= 0x10;
    t3aa = t3aa + ta12c;
    M2C_FIELD(p26, u16 *, 0x3A) = (u16) t3aa;
    g_TaskNodePool->active = 1;
exit:
    return ret;
}
