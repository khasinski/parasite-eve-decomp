#include "common.h"
extern char *g_CurrentEntity;
int Task_RotateEntityAngle(int **arg0) {
    int *src;
    char *ptr;
    int value;
    int delta;
    char frame[8];
    src = arg0[0];
    ptr = g_CurrentEntity;
    delta = src[0];
    value = *(u16 *)(ptr + 0x3A) + delta;
    *(u16 *)(ptr + 0x3A) = value;
    if ((short)value >= 0x1001) {
        int adjusted;
        adjusted = value - 0x1000;
        *(u16 *)(ptr + 0x3A) = adjusted;
    }
    ptr = g_CurrentEntity;
    {
        int signed_value;
        signed_value = *(short *)(ptr + 0x3A);
        value = signed_value;
        if (signed_value < 0) {
            signed_value = 0x1000;
            signed_value = value + signed_value;
            *(u16 *)(ptr + 0x3A) = signed_value;
        }
    }
    return 1;
}
