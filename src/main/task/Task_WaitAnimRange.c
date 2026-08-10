#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;

int Task_WaitAnimRange(void) {
    FieldActor *entity;
    int *node;
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
    node[4] = 1;
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
