extern signed char g_GameState[];

int Overlay_GetAudioSlotByKey(int arg0) {
    register signed char *entry asm("$3");
    int i;

    i = 0;
    entry = g_GameState;
    while (i < 2) {
        if (entry[0xDC] == arg0) {
            return entry[0xDD];
        }
        i++;
        entry += 2;
    }

    return -1;
}
