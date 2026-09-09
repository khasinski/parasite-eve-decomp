#include "common.h"
#include "pe1/field_actor.h"
extern FieldActor *g_CurrentEntity;

int Geo_PointInPoly(int arg0, int arg1, int arg2, int arg3);

int Entity_CallAction(int **arg0) {
    int **args;
    register int *arg0_ptr asm("$2");
    int *arg1_ptr;
    int *arg2_ptr;
    int arg2;
    int arg1;
    int arg3;
    register int base asm("$8");
    register FieldActor *current asm("$2");
    int result;

    args = arg0;
    arg0_ptr = args[3];
    arg1_ptr = args[1];
        arg2_ptr = args[2];
    arg2 = *arg0_ptr;
    current = g_CurrentEntity;
    asm volatile("" ::: "memory");
    arg1 = *(volatile int *)arg1_ptr;
        arg3 = *(u16 *)arg2_ptr;
    base = (int)current->script_base;
    arg0_ptr = args[0];
    arg2 <<= 1;
    result = Geo_PointInPoly(*arg0_ptr, arg1, base + arg2, arg3);
    *args[4] = result;
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
