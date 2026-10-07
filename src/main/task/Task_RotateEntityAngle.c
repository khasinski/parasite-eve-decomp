#include "common.h"
extern char *g_CurrentEntity;

int Task_RotateEntityAngle(int **arg0) {
    int *src;
    char *ptr;
    register int value asm("$3");
    char frame[8];

    src = arg0[0];
    ptr = g_CurrentEntity;
    value = src[0];
    value = *(u16 *)(ptr + 0x3A) + value;
    *(u16 *)(ptr + 0x3A) = value;
    if ((short)value >= 0x1001) {
        register int adjusted asm("$2");
        adjusted = value - 0x1000;
        *(u16 *)(ptr + 0x3A) = adjusted;
    }

    ptr = g_CurrentEntity;
    {
        int signed_value;
        signed_value = *(short *)(ptr + 0x3A);
        value = signed_value;
        if (signed_value < 0) {
            signed_value = value + 0x1000;
            *(u16 *)(ptr + 0x3A) = signed_value;
        }
    }
    return 1;
}
