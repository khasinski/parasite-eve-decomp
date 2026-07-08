#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;
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
