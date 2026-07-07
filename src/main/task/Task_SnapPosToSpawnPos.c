/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 */

extern char *g_CurrentEntity[];

int Task_SnapPosToSpawnPos(void) {
    char *ptr;
    register int x asm("$2");
    int y;

    ptr = g_CurrentEntity[0];
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
