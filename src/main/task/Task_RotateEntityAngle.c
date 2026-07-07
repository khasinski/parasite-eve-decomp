/* CC1_FLAGS: -g3 */

typedef unsigned short u16;

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
        register int copy asm("$3");
        signed_value = *(short *)(ptr + 0x3A);
        copy = signed_value;
        if (signed_value < 0) {
            signed_value = copy + 0x1000;
            *(u16 *)(ptr + 0x3A) = signed_value;
        }
    }
    return 1;
}
