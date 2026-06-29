extern char *g_CurrentEntity;
extern char *g_PlayerEntity;
extern short D_800BCFFE;

int Task_SetEntityMoveSpeed(int **arg0) {
    int **args = arg0;
    char *current = g_CurrentEntity;
    char *selected = g_PlayerEntity;
    int value = *args[0] >> 4;

    *(short *)(current + 0x26) = value;
    if (current == selected) {
        int scaled;

        scaled = ((*args[0] >> 4) * 3) << 7;

        if (scaled < 0) {
            scaled += 0xFFF;
        }
        D_800BCFFE = scaled >> 12;
    }
    return 1;
}
