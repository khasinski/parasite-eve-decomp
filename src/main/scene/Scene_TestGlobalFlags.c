
extern int g_GameStateFlags;

int Scene_TestGlobalFlags(int **arg0) {
    int flags;
    int mask;

    flags = g_GameStateFlags;
    mask = *arg0[0];
    if (flags & mask) {
        *arg0[1] = 1;
    } else {
        *arg0[1] = 0;
    }
    return 1;
}
