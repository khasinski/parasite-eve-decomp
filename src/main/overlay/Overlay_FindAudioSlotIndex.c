typedef signed char s8;

extern char g_GameState;

int Overlay_FindAudioSlotIndex(int id) {
    int i = 0;
    char *ptr = &g_GameState;

    do {
        if (*(s8 *)(ptr + 0xDC) == id) {
            return i;
        }
        i++;
        ptr += 2;
    } while (i < 2);

    return -1;
}
