extern char g_GameState[];

int Overlay_RegisterAudioSlot(int arg0, int arg1, int arg2, int arg3) {
    char *base;
    char *ptr;

    base = g_GameState;
    ptr = &base[arg0 * 2];
    ptr[0xDC] = (char) arg1;
    ptr[0xDD] = (char) arg2;
    base[arg0 + 0xFE] = (char) arg3;

    if (arg0 == 0) {
        *(int *) base |= 0x40;
    } else if (arg0 == 1) {
        *(int *) base |= 0x80;
    }

    return 0;
}
