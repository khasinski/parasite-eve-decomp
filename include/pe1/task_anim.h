#ifndef PE1_TASK_ANIM_H
#define PE1_TASK_ANIM_H

typedef struct TaskAnimEntry {
    unsigned char field00;
    unsigned char field01;
    unsigned char field02;
    unsigned char field03;
    int coord_x;
    int coord_y;
    unsigned char field0C[4];
} TaskAnimEntry;

/* Task animation object: a thin wrapper over a base pointer. */
typedef struct TaskAnimObj {
    unsigned char *base;              /* 0x00 */
} TaskAnimObj;

#endif /* PE1_TASK_ANIM_H */
