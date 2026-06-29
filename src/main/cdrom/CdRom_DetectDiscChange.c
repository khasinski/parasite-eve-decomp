extern unsigned int g_GameState;
extern unsigned char g_SceneAreaType;
extern unsigned char g_SavedSceneAreaType;
extern unsigned char g_DiscChangeFlags;

int CdRom_DetectDiscChange(void) {
    unsigned int *state = &g_GameState;
    int offset = g_SceneAreaType - 0xA;

    if ((unsigned int)offset < 5) {
        int value = g_SavedSceneAreaType;

        if (g_SceneAreaType != (unsigned char)value) {
            *state |= 0x200000;
        }

        if (((unsigned int)offset >> 1) != ((value - 0xA) / 2)) {
            g_DiscChangeFlags |= 4;
        }
    }

    return 0;
}
