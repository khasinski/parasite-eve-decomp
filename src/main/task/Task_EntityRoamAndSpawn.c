#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Gte_Atan2(int arg0, int arg1);

int Task_ClearEntityRoamFlag(void) {
    g_CurrentEntity->flags &= 0xFFFDFFFF;
    return 1;
}

int Task_SetEntityRoamFlag(void) {
    g_CurrentEntity->flags |= 0x20000;
    return 1;
}

int Task_GetAngleToPoint(int **arg0) {
    int angle;
    int dx;
    int dy;

    dx = (((int *)g_CurrentEntity)[10] - *arg0[0]) >> 16;
    dy = (((int *)g_CurrentEntity)[12] - *arg0[1]) >> 16;
    angle = 0x1400 - Gte_Atan2(dy, dx);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= ((short *)g_CurrentEntity)[0x1D];
    if (angle < 0) {
        angle += 0x1000;
    }
    *arg0[2] = angle;
    return 1;
}


int Task_SnapPosToSpawnPos(void) {
    char *ptr;
    register int x asm("$2");
    int y;

    ptr = (char *)g_CurrentEntity;
    x = *(short *)(ptr + 0x21C);
    y = *(short *)(ptr + 0x220);
    x <<= 16;
    *(int *)(ptr + 0x28) = x;
    x = *(short *)(ptr + 0x21E);
    y <<= 16;
    *(int *)(ptr + 0x30) = y;
    x <<= 16;
    *(int *)(ptr + 0x2C) = x;
    return 1;
}
