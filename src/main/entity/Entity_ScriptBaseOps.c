#include "common.h"
#include "pe1/field_collision.h"
#include "pe1/field_actor.h"
extern FieldActor *g_CurrentEntity;


int Entity_CallAction(int **arg0)
{
    int **args;
    int *offsetInput;
    int **xSlot;
    int polygonOffset;
    u8 *base;
    FieldActor *current;
    args = arg0;
    offsetInput = args[3];
    polygonOffset = *offsetInput;
    current = g_CurrentEntity;
    xSlot = &args[0];
    base = current->script_base;
    polygonOffset <<= 1;
    *args[4] = Geo_PointInPoly(**xSlot, *args[1], (const PolygonVertex *) (base + polygonOffset), *(u16 *)args[2]);
    return 1;
}
extern FieldActor *g_CurrentEntity;
extern int *D_8009D248;
extern short D_8009D1CC;

int Inv_CountTotal(void);
int Inv_GetAyaSlotLimit(void);

int Task_SetInventorySlotPointer(int **arg0) {
    D_8009D248 = (int *)(g_CurrentEntity->script_base + (*arg0[0] << 1));
    D_8009D1CC = *arg0[1];
    return 1;
}

int Task_GetInventoryTotal(int **arg0) {
    *arg0[0] = Inv_CountTotal();
    return 1;
}

int Task_GetInventorySlotLimit(int **arg0) {
    *arg0[0] = Inv_GetAyaSlotLimit();
    return 1;
}
