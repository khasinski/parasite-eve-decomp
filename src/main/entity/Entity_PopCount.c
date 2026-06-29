#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;
extern int *D_8009D248;
extern short D_8009D1CC;
extern char D_800BCFFC;

int Entity_PopCount(int **arg0) {
    int value;
    int count;

    value = *arg0[0];
    count = 0;
    while (value != 0) {
        value &= value - 1;
        count++;
    }

    *arg0[1] = count;
    return 1;
}

int func_8001A32C(void) {
    g_CurrentEntity->flags |= 2;
    return 1;
}

int func_8001A350(void) {
    g_CurrentEntity->flags &= -3;
    return 1;
}

int func_8001A374(int **arg0) {
    D_800BCFFC = *arg0[0];
    return 1;
}
