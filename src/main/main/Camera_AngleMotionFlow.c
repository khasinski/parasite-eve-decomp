#include "common.h"
#include "pe1/global_slot.h"
#include "pe1/task_node.h"
#include "pe1/field_actor.h"

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

extern Pe1GlobalSlot D_8009D254;
extern FieldActor *D_8009D2F0[];
extern int *D_8009CE00;

int Math_FixedMul(int a, int b);
int ratan2(int y, int x);
int rsin(int angle);
int rcos(int angle);

int Camera_TrackRelativeOffset(int **args) {
    FieldActor *speed_entity;
    FieldActor *motion_entity;
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
    current_x = speed_entity->pos_x;
    current_z = speed_entity->pos_z;
    if (speed_entity != D_8009D254.value.pointer) {
        movement_speed = speed_entity->move_factor;
    } else {
        movement_speed = Math_FixedMul(0x50000, speed_entity->move_factor);
    }
    movement_speed = Math_FixedMul(movement_speed, D_8009D2F0[0]->move_speed << 4);

    init_state = D_8009D300;
    if ((init_state->flags & 0x20) == 0) {
        turn_speed = *args[2];
        target_x = current_x + *args[0];
        target_z = current_z + *args[1];
        init_state->flags |= 0x20;
        init_state->field_20 = 0;
        init_state->target14 = turn_speed;
        init_state->target1c = target_z;
        init_state->target18.coordinate = target_x;
    } else {
        turn_speed = init_state->target14;
        target_x = init_state->target18.coordinate;
        target_z = init_state->target1c;
    }

    target_angle = (u16)(0x1400 - ratan2(current_z - target_z,
                                             current_x - target_x));
    target_angle &= 0xFFF;
    desired_angle = target_angle;
    if (turn_speed != 0) {
        current_angle = D_8009D2F0[0]->rot_y;
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
        D_8009D2F0[0]->rot_y = target_angle;
    }

    D_8009D2F0[0]->motion_x = Math_FixedMul(-movement_speed, rsin(target_angle) << 4);
    movement_z = Math_FixedMul(-movement_speed, rcos(target_angle) << 4);
    current_x = (target_x - current_x) >> 16;
    current_z = (target_z - current_z) >> 16;
    remaining_distance = current_x * current_x + current_z * current_z;
    motion_entity = D_8009D2F0[0];
    current_x = S16_AT(motion_entity, 0x6A);
    current_z = movement_z >> 16;
    movement_distance = current_x * current_x + current_z * current_z;
    motion_entity->motion_z = movement_z;
    if (movement_distance >= remaining_distance) {
        camera_state = D_8009D300;
        motion_entity->pos_x = target_x;
        motion_entity->pos_z = target_z;
        motion_entity->motion_x = 0;
        motion_entity->motion_z = 0;
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

extern s32 g_SceneDataTable0;
extern FieldActor *g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])
extern TaskNode *g_TaskNodePool;

s32 Camera_StepAngleFade(s32 **arg0) {
    register s32 temp_a1 asm("$5");
    s32 ta1c;
    register s32 ta1b asm("$6");
    s32 temp_t1;
    TaskNode *p300;
    s32 tv1a;
    u16 temp_a3;
    s32 desiredAngle;
    register s32 phaseValue asm("$2");
    register s32 turnIncrement asm("$7");
    register s32 ta1d asm("$4");
    s32 temp_a1_2;
    s32 sv0;
    register s32 temp_v1x asm("$3");
    s32 var_v1;
    register TaskNode *p13 asm("$2");
    s32 tv0;
    s32 *tv0p;
    register u16 *pa3 asm("$3");
    s32 tor;
    s32 vext;
    s32 vext2;
    s32 sa3a;
    s32 sa3b;
    s32 sa3c;
    s32 temp_v0_2;
    s32 tve;
    TaskNode *p24;
    FieldActor *p26;
    s32 t3aa;
    s32 tmask4;
    s32 sa12a;
    s32 sa12b;
    s32 temp_v1_2;
    s32 temp_v0_3;
    s32 temp_v0_4;

    phaseValue = (u16)g_CurrentEntity->rot_y;
    temp_a1 = phaseValue & 0xFFF;
    p300 = g_TaskNodePool;
    temp_t1 = p300->flags;
    ta1c = temp_a1;
    if (!(temp_t1 & 0x20)) {
        ta1b = temp_a1;
                tv0p = arg0[0];
        pa3 = (u16 *)arg0[1];
        tv0 = *tv0p;
        temp_a3 = *pa3;
        temp_v1x = tv0 & 0xFFF;
        tv1a = temp_v1x;
        __asm__("" : "=r"(tv1a) : "0"(tv1a));
        desiredAngle = temp_v1x;
        if (ta1b == tv1a) {
            phaseValue = 1;
            goto exit;
        }
        tor = temp_t1 | 0x20;
        p300->flags = (u16) tor;
                phaseValue = tv1a - ta1b;
        if (phaseValue >= 0) {
            var_v1 = temp_v1x - temp_a1;
        } else {
            var_v1 = temp_a1 - temp_v1x;
        }
        if ((s16) desiredAngle < ta1c) {
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
        phaseValue = (s16) desiredAngle;
        g_TaskNodePool->target18.coordinate = phaseValue;
    }
    p13 = g_TaskNodePool;
    ta1d = ta1c;
    temp_a1_2 = p13->target14;
    phaseValue = p13->target18.coordinate;
    __asm__("" : "=r"(phaseValue) : "0"(phaseValue));
    desiredAngle = phaseValue;
    __asm__("" : "=r"(desiredAngle) : "0"(desiredAngle));
    sv0 = (s16) phaseValue;
    turnIncrement = temp_a1_2;
    if (ta1d < sv0) goto chk2;
    sa12a = (s16) temp_a1_2;
    if (sv0 < (ta1d + sa12a)) goto chk2;
    goto block_24;
chk2:
    __asm__("" : "=r"(turnIncrement) : "0"(turnIncrement));
    if (sv0 < ta1d) {
        tve = turnIncrement << 0x10;
        goto inner;
    }
    sa12b = temp_a1_2 << 0x10;
    phaseValue = ta1d + (sa12b >> 0x10);
        if (phaseValue < sv0) {
        tve = turnIncrement << 0x10;
        goto inner;
    }
    goto block_24;
inner:
    {
        temp_v0_2 = tve >> 0x10;
        temp_v1_2 = ta1c + temp_v0_2;
        if (temp_v0_2 > 0) {
            if (temp_v1_2 >= 0x1001) {
                if ((temp_v1_2 & 0xFFF) < (s16) desiredAngle) {
                    goto block_21;
                }
                goto block_24;
            }
        }
block_21:
        temp_v0_3 = (s16) turnIncrement;
        if (temp_v0_3 < 0) {
            temp_v0_4 = ta1c + temp_v0_3;
            if (temp_v0_4 < 0) {
                tmask4 = temp_v0_4 & 0xFFF;
                if ((s16) desiredAngle < tmask4) {
                    goto block_26;
                }
                goto block_24;
            }
        }
        goto block_26;
    }
block_24:
    p24 = g_TaskNodePool;
    g_CurrentEntity->rot_y = (u16) desiredAngle;
    p24->flags = (u16) (p24->flags & 0xFFDF);
    phaseValue = 1;
    goto exit;
block_26:
    phaseValue = 0;
        p26 = g_CurrentEntity;
    t3aa = (u16)p26->rot_y;
    g_SceneDataTable0 -= 0x10;
    t3aa = t3aa + turnIncrement;
    p26->rot_y = (u16) t3aa;
    g_TaskNodePool->active = 1;
exit:
    return phaseValue;
}
