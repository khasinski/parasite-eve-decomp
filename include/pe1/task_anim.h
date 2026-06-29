#ifndef PE1_TASK_ANIM_H
#define PE1_TASK_ANIM_H

/* Task animation object: a thin wrapper over a base pointer; all access is raw
 * pointer arithmetic off base (Task_SetObjAnimEntry*). */
typedef struct TaskAnimObj {
    unsigned char *base;              /* 0x00 */
} TaskAnimObj;

#endif /* PE1_TASK_ANIM_H */
