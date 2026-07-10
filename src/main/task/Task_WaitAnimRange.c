/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct TaskAnimEntity {
    unsigned char pad_00[0x0F];
    u8 anim_frame_max;
    unsigned char pad_10[0x02];
    u16 anim_frame_target;
    int anim_pos;
    unsigned char pad_18[0x04];
    int anim_step;
} TaskAnimEntity;

extern TaskAnimEntity *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;

int Task_WaitAnimRange(void) {
    TaskAnimEntity *entity;
    int *node;
    register int current asm("$5");
    register int target asm("$6");
    int next;
    int frame;
    int max_frame;
    register int cmp asm("$2");
    register int divisor asm("$2");

    entity = g_CurrentEntity[0];
    node = g_TaskNodePool;
    current = entity->anim_pos;
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

    max_frame = g_CurrentEntity[0]->anim_frame_max;
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
