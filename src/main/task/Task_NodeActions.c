#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */

#include "pe1/task_node.h"
#include "pe1/field_actor.h"
#include "pe1/global_slot.h"

extern TaskNode *g_TaskNodePool;
extern TaskNode *g_TaskNodeFreeListHead;
extern u16 g_TaskNodeSeqCounter;
extern FieldActor *g_CurrentEntity[];
extern FieldActor *g_CurrentEntityForDirection[] asm("g_CurrentEntity");

int Task_SpawnChildNode(int **args) {
    TaskNode *node = g_TaskNodePool;

    if ((node->flags & 3) != 0) {
        register int *value_ptr asm("$2");
        register int *state asm("$5");
        TaskNode *entry;
        u16 seq;
        register int value asm("$4");
        int base;
        TaskNode *next;

        value_ptr = args[0];
        state = (int *)g_CurrentEntity[0];
        entry = g_TaskNodeFreeListHead;
        seq = g_TaskNodeSeqCounter;
        value = *value_ptr;
        base = state[0x9C / 4];
        asm volatile("" : "=r"(entry) : "0"(entry), "r"(value), "r"(base));
        next = entry->next;
        entry->prev = 0;
        entry->next = 0;
        entry->trigger_value = 0;
        entry->next_value = 0;
        entry->active = 1;
        entry->seq = seq;
        entry->flags = 0;
        asm volatile("" : "=r"(value) : "0"(value));
        value <<= 1;
        value += base;
        entry->current = value;
        {
            TaskNode *head = (TaskNode *)state[0xA8 / 4];
            seq++;
            g_TaskNodeSeqCounter = seq;
            g_TaskNodeFreeListHead = next;
            entry->next = head;
            if (head != 0) {
                head->prev = entry;
            }
            ((TaskNode **)g_CurrentEntity[0])[0xA8 / 4] = entry;
        }
    } else {
        register TaskNode *entry asm("$5") = g_TaskNodeFreeListHead;
        register int *value_ptr asm("$2") = args[0];
        register int *state asm("$4") = g_CurrentEntity[0];
        TaskNode *next = entry->next;
        int value;

        value = *value_ptr;
        g_TaskNodeFreeListHead = next;
        asm volatile("" : : : "memory");
        value = (value << 1) + state[0x9C / 4];
        if (node != 0) {
            entry->prev = node;
            next = node->next;
            entry->next = next;
            if (next != 0) {
                next->prev = entry;
            }
            node->next = entry;
        } else {
            entry->prev = 0;
            entry->next = 0;
        }
        entry->current = value;
        {
            u16 seq = g_TaskNodeSeqCounter;
            entry->trigger_value = 0;
            entry->next_value = 0;
            entry->active = 1;
            entry->flags = 0;
            g_TaskNodeSeqCounter = seq + 1;
            entry->seq = seq;
        }
    }
    return 1;
}

extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;
extern FieldActor *g_CurrentEntity[];
/* Same symbol; keep the post-call address calculation independent in GCC. */
extern FieldActor *g_CurrentEntityAfterAction[] asm("g_CurrentEntity");

void Entity_SetActionMode(char *arg0, int arg1);

int Task_SetEntityActionAndWait(int **arg0) {
    char *node = (char *)g_TaskNodePool;
    int flags = *(u16 *)(node + 8);

    if ((flags & 0x20) == 0) {
        int *ptr;
        int mode;

        *(u16 *)(node + 8) = flags | 0x20;
        ptr = arg0[0];
        {
            char *state;
            state = g_CurrentEntity[0];
            mode = *(u16 *)ptr;
            Entity_SetActionMode(state, mode);
        }
        {
            char *state;
            state = g_CurrentEntityAfterAction[0];
            *(int *)(state + 0x98) &= -0x101;
        }
        goto pop_state;
    } else {
        char *state;
        int keep;

        state = g_CurrentEntity[0];
        if (*(u8 *)(state + 0xF) == 0) {
            goto finish;
        }
        if (*(int *)(state + 0x1C) >= 0) {
            register unsigned int lhs asm("$2") = *(unsigned int *)(state + 0x14);
            unsigned int rhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        } else {
            unsigned int rhs = *(unsigned int *)(state + 0x14);
            unsigned int lhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        }
        if (keep == 0) {
            goto pop_state;
        }

finish:
        *(u16 *)(node + 8) = flags & 0xFFDF;
        return 1;
    }

pop_state:
    {
        int cursor = g_SceneDataTable0;
        int *active_node = (int *)g_TaskNodePool;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
        active_node[4] = 1;
        return 0;
    }
}

/* Animation-range wait consumes the active TaskNode and FieldActor view. */
int Task_WaitAnimRange(void) {
    FieldActor *entity;
    TaskNode *node;
    int current;
    int target;
    int next;
    int frame;
    int max_frame;
    int cmp;
    register int divisor asm("$2");

    entity = g_CurrentEntity[0];
    node = g_TaskNodePool;
    current = entity->anim.fixed;
    asm volatile("" : "=r"(current) : "0"(current));
    target = entity->anim_frame_target;
    node->active = 1;
    if ((current >> 16) == target) {
        return 0;
    }

    target <<= 16;
    next = current + entity->anim_step;
    if (current < target && target < next) {
        return 0;
    }
    cmp = target < current;
    if (cmp && next < target) {
        return 0;
    }

    max_frame = g_CurrentEntity[0]->action;
    frame = next >> 16;
    if (max_frame < frame) {
        divisor = max_frame + 1;
        next = (frame % divisor) << 16;
        cmp = target < next;
        if (cmp) {
            return 0;
        }
    } else {
        divisor = max_frame + 1;
        max_frame <<= 16;
        if (next < 0) {
            next = (divisor + (frame % divisor)) << 16;
            if (max_frame >= target) {
                cmp = next < target;
                if (cmp) {
                    return 0;
                }
            }
        }
    }

    g_SceneDataTable0 -= 8;
    return 0;
}

/* Point-turn and movement handlers use the same active TaskNode layout. */
int Task_TurnTowardPointStep(int **arg0) {
    TaskNode *node = g_TaskNodePool;
    int flags = node->flags;
    int x;
    int y;
    int step;
    register int angle asm("$3");
    register int stepped asm("$6");
    int original;

    {
        int active = flags & 0x20;
        if (active != 0) {
            goto cached_args;
        }
    }
    {
        int new_flags;

        x = *arg0[0];
        y = *arg0[1];
        step = *arg0[2];
        new_flags = flags | 0x20;
        node->flags = new_flags;
        node->target14 = x;
        node->target1c = step;
        node->target18 = y;
        goto have_args;
    }

cached_args:
    {
        x = node->target14;
        y = node->target18;
        step = node->target1c;
    }

have_args:
    {
        char *state;
        register int dx asm("$6");
        int dy;
        register int tmp asm("$2");

        state = g_CurrentEntityForDirection[0];
        tmp = *(int *)(state + 0x28);
        dx = (tmp - x) >> 16;
        dy = (*(int *)(state + 0x30) - y) >> 16;
        if ((dx | dy) == 0) {
            return 1;
        }
        angle = 0x1400 - Gte_Atan2(dy, dx);
    }

    {
        char *state;
        state = g_CurrentEntity[0];
        angle &= 0xFFF;
        original = *(short *)(state + 0x3A);
    }
    stepped = angle;
    if (angle == original) {
        goto finish;
    }

    {
        int current = original;
        register int desired asm("$3") = angle;
        register int out asm("$6") = stepped;
        int delta;

        if (current < desired) {
            delta = desired - current;
            if (delta < 0x800) {
                if (step < delta) {
                    out = current + step;
                }
            } else if (step < delta) {
                out = current - step;
                if (out < 0) {
                    int wrap = current + 0x1000;
                    wrap = wrap - desired;
                    if (wrap < step) {
                        out = desired;
                    }
                }
            }
        } else {
            delta = current - desired;
            if (delta < 0x800) {
                if (step < delta) {
                    out = current - step;
                }
            } else if (step < delta) {
                out = current + step;
                if (out >= 0x1001) {
                    int wrap = desired + 0x1000;
                    wrap = wrap - current;
                    if (wrap < step) {
                        out = desired;
                    }
                }
            }
        }
        stepped = out;
    }
    {
        char *state;
        state = g_CurrentEntity[0];
        stepped &= 0xFFF;
        *(u16 *)(state + 0x3A) = stepped;
    }
    if (stepped != angle) {
        int cursor = g_SceneDataTable0;
        char *active = g_TaskNodePool;
        cursor -= 0x14;
        g_SceneDataTable0 = cursor;
        ((TaskNode *)active)->active = 1;
        return 0;
    }

finish:
    {
        char *active = g_TaskNodePool;
        int f = *(u16 *)(active + 8);
        f &= 0xFFDF;
        *(u16 *)(active + 8) = f;
        return 1;
    }
}

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

int Task_MoveTowardPoint(int **args) {
    u8 *speed_entity;
    u8 *motion_entity;
    u8 *camera_state;
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

    if ((D_8009D300->flags & 0x20) == 0) {
        target_x = *args[0];
        target_z = *args[1];
        if (current_x == target_x && current_z == target_z) {
            return 1;
        }
        turn_speed = *args[2];
        D_8009D300->target14 = target_x;
        D_8009D300->target18 = target_z;
        D_8009D300->target1c = turn_speed;
        D_8009D300->flags |= 0x20;
    } else {
        camera_state = D_8009D300;
        target_x = S32_AT(camera_state, 0x14);
        target_z = S32_AT(camera_state, 0x18);
        turn_speed = S32_AT(camera_state, 0x1C);
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
        D_8009D300->flags &= 0xFFDF;
        return 1;
    } else {
        D_8009CE00 -= 5;
        D_8009D300->active = 1;
        return 0;
    }
}
