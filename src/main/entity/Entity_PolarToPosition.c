
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int rsin(int arg0);
int rcos(int arg0);
int Math_FixedMul(int arg0, int arg1);

int Entity_PolarToPosition(int **arg0)
{
    int **args;
    short angle;
    int radius;
    int *ptr;
    register int value asm("$2");
    int radiusInput;
    int *angleInput;
    FieldActor *current_v1;
    FieldActor *current_v0;
    int base;
    args = arg0;
    angleInput = args[1];
    ptr = angleInput;
    value = *ptr;
    angle = 0x1400 - value;
    ptr = args[0];
    radiusInput = *ptr;
    radius = radiusInput;
    value = rcos(angle);
    radius = -radius;
    value = Math_FixedMul(radius, value << 4);
    {
        int *dst;
        dst = args[2];
        *dst = value;
    }
    value = rsin(angle);
    value = Math_FixedMul(radius, value << 4);
    {
        int *dst;
        dst = args[3];
        *dst = value;
    }
    {
        int *dst;
        dst = args[2];
        current_v1 = g_CurrentEntity;
        value = *dst;
        value += current_v1->pos_x;
        *dst = value;
    }
    {
        int *dst;
        int loaded;
        dst = args[3];
        current_v0 = g_CurrentEntity;
        loaded = *dst;
        base = current_v0->pos_z;
        loaded += base;
        *dst = loaded;
    }
    return 1;
}
