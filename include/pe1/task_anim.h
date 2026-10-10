#ifndef PE1_TASK_ANIM_H
#define PE1_TASK_ANIM_H

#include "pe1/battle_entity_anim.h"

/* Animation setup only needs the actor's first pointer. */
typedef struct TaskAnimObj {
    EntityAnimEventCore *core;
} TaskAnimObj;

#endif
