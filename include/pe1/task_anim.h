#ifndef PE1_TASK_ANIM_H
#define PE1_TASK_ANIM_H

#include "pe1/battle_entity_anim.h"

/* Animation setup only needs the actor's first pointer. */
typedef struct TaskAnimObj {
    EntityAnimEventCore *core;
} TaskAnimObj;

/* The twelve-argument entry uses C89 default promotions at the call boundary;
 * the definition retains byte/halfword parameter values within the function. */
void Task_SetObjAnimEntry12(TaskAnimObj *obj, int index, int effectType,
                          int enterMode, int exitMode, int power,
                          int parameter0, int parameter1, int parameter2,
                          int parameter3, int category, int frame);
void Task_SetObjAnimEntry5(TaskAnimObj *obj, int index, int effectType,
                         int enterMode, u8 exitMode, u16 power);

void Battle_SetEntryCoords(TaskAnimObj *obj, u8 index, int enterStep, int exitStep);

#endif
