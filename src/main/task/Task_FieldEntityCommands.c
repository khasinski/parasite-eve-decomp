extern int g_GameState;

#include "pe1/field_actor.h"
#include "pe1/task_anim.h"
#include "pe1/font.h"

extern FieldActor *g_CurrentEntity;

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
    g_CurrentEntity->flags |= 0x80;
    return 1;
}

int Entity_ClearCurrentFlag80(void) {
    g_CurrentEntity->flags &= -0x81;
    return 1;
}

int Entity_SetCurrentFlag4000(void) {
    g_CurrentEntity->flags |= 0x4000;
    return 1;
}

int Entity_ClearCurrentFlag4000(void) {
    g_CurrentEntity->flags &= -0x4001;
    return 1;
}

extern short *g_EntityCollisionWallSlot;
extern short g_EntityCollisionWallParam;

extern int g_FieldMoveLock;

int Task_SetAnimSlotPointer(int **arg0) {
    int index = *arg0[0];

    g_EntityCollisionWallSlot = (short *)(g_CurrentEntity->script_base + index * 2);
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

void Entity_ResolvePosition(BattleEntity *actor, int index);


void Task_EnableMovement(void);

void Task_DisableMovement(void);

int Entity_ResolveCurrentPosition(unsigned short **arg0) {
    Entity_ResolvePosition((BattleEntity *)g_CurrentEntity, *arg0[0]);
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
  new_var = g_CurrentEntity;
  current = new_var;
  args = arg0;
  src = args[0];
  radius = *src;
  value = rsin((short) current->rot_y);
  radius = -radius;
  value = Math_FixedMul(radius, value << 4);
  current_v1 = g_CurrentEntity;
  dst = args[1];
  *dst = g_CurrentEntity->pos_x + value;
  current = g_CurrentEntity;
  value = rcos((short) current->rot_y);
  value = Math_FixedMul(radius, value << 4);
  current_v1 = g_CurrentEntity;
  dst = args[2];
  *args[2] = current_v1->pos_z + value;
  return 1;
}

/* Contiguous animation-entry commands share the current actor. */



void Render_SetEntryScrolled(int arg0, int arg1, int arg2, int arg3);
void Render_SetEntryMirrored(int arg0, int arg1, int arg2, int arg3);

int Task_SetBattleEntryCoords(int **arg0) {
    Battle_SetEntryCoords((TaskAnimObj *)g_CurrentEntity, *(unsigned char *)arg0[0], *arg0[1], *arg0[2]);
    return 1;
}

int func_80019904(void) {
    g_CurrentEntity->flags &= -2;
    return 1;
}

int func_80019928(void) {
    g_CurrentEntity->flags |= 1;
    return 1;
}

int func_8001994C(int **arg0) {
    Render_SetEntryScrolled(*arg0[0], *arg0[1], *arg0[2], *arg0[3]);
    return 1;
}

int func_8001998C(int **arg0) {
    Render_SetEntryMirrored(*arg0[0], *arg0[1], *arg0[2], *arg0[3]);
    return 1;
}

int func_800199CC(int **arg0) {
    g_CurrentEntity->render_object.script_value9a = *arg0[0];
    g_CurrentEntity->render_object.flags_9C |= 0x10;
    return 1;
}

int func_800199F8(void) {
    g_CurrentEntity->render_object.flags_9C &= 0xFFEF;
    return 1;
}

int func_80019A1C(int **arg0) {
    g_CurrentEntity->render_object.script_value9a = *arg0[0];
    g_CurrentEntity->render_object.script_param97 = *arg0[1];
    g_CurrentEntity->render_object.script_param98 = *arg0[2];
    g_CurrentEntity->render_object.script_param99 = *arg0[3];
    g_CurrentEntity->render_object.flags_9C |= 8;
    return 1;
}

int func_80019A9C(void) {
    g_CurrentEntity->render_object.flags_9C &= 0xFFF7;
    return 1;
}

int func_80019AC0(void) {
    g_CurrentEntity->flags |= 0x400;
    return 1;
}

int func_80019AE4(void) {
    g_CurrentEntity->flags &= -0x401;
    return 1;
}
