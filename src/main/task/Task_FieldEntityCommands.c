extern int g_GameState;

#include "pe1/field_actor.h"

extern char *g_CurrentEntity;

int Task_SetSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr |= 0x2000;
    return 1;
}

int Task_ClearSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr &= -0x2001;
    return 1;
}

int Entity_SetCurrentFlag80(void) {
    ((FieldActor *)g_CurrentEntity)->flags |= 0x80;
    return 1;
}

int Entity_ClearCurrentFlag80(void) {
    ((FieldActor *)g_CurrentEntity)->flags &= -0x81;
    return 1;
}

int Entity_SetCurrentFlag4000(void) {
    ((FieldActor *)g_CurrentEntity)->flags |= 0x4000;
    return 1;
}

int Entity_ClearCurrentFlag4000(void) {
    ((FieldActor *)g_CurrentEntity)->flags &= -0x4001;
    return 1;
}

extern char *g_CurrentEntity;
extern short *g_EntityCollisionWallSlot;
extern short g_EntityCollisionWallParam;

extern int g_FieldMoveLock;

int Task_SetAnimSlotPointer(int **arg0) {
    int index = *arg0[0];

    g_EntityCollisionWallSlot = (short *)(*(char **)(g_CurrentEntity + 0x9C) + index * 2);
    g_EntityCollisionWallParam = *arg0[1];
    return 1;
}

int Task_SetInputFlagBit2(void) {
    g_FieldMoveLock |= 4;
    return 1;
}

int Task_ClearInputFlagBit2(void) {
    g_FieldMoveLock &= -5;
    return 1;
}

void Entity_ResolvePosition(char *actor, int index);

int Menu_GetEquipSlotStateOrIndex(void);

void Task_EnableMovement(void);

void Task_DisableMovement(void);

int Entity_ResolveCurrentPosition(unsigned short **arg0) {
    Entity_ResolvePosition(g_CurrentEntity, *arg0[0]);
    return 1;
}

int Task_GetFloorNumber(int **arg0) {
    *arg0[0] = (unsigned char)Menu_GetEquipSlotStateOrIndex();
    return 1;
}

int Task_SetMoveFlag(void) {
    Task_EnableMovement();
    return 1;
}

int Task_ClearMoveFlag(void) {
    Task_DisableMovement();
    return 1;
}

extern char *g_CurrentEntity;
int rsin(int arg0);
int rcos(int arg0);
int Math_FixedMul(int arg0, int arg1);
int Entity_PolarToPosition2(int **arg0)
{
  int **args;
  FieldActor *new_var;
  int radius;
  FieldActor *current;
  FieldActor *current_v1;
  register int *src;
  int *dst;
  int value;
  new_var = (FieldActor *)g_CurrentEntity;
  current = new_var;
  args = arg0;
  src = args[0];
  radius = *src;
  value = rsin((short) current->rot_y);
  radius = -radius;
  value = Math_FixedMul(radius, value << 4);
  current_v1 = (FieldActor *)g_CurrentEntity;
  dst = args[1];
  *dst = ((FieldActor *)g_CurrentEntity)->pos_x + value;
  current = (FieldActor *)g_CurrentEntity;
  value = rcos((short) current->rot_y);
  value = Math_FixedMul(radius, value << 4);
  current_v1 = (FieldActor *)g_CurrentEntity;
  dst = args[2];
  *args[2] = current_v1->pos_z + value;
  return 1;
}
