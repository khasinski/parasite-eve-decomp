extern volatile int D_800A76A0;
extern volatile int g_GameTimeTable;
extern volatile int D_800A76A8;

extern volatile int D_800A76B8;
extern volatile int g_PlayTimeSeconds;
extern volatile int D_800A76C0;

extern volatile int D_800A76AC;
extern volatile int D_800A76B0;
extern volatile int D_800A76B4;

void GameTime_InitCounter0(void) {
    int value;

    value = 0x1499700;
    D_800A76A8 = 0;
    D_800A76A8 = value;
    value = 1;
    g_GameTimeTable = 0;
    D_800A76A0 = 0;
    D_800A76A0 = value;
}

void GameTime_InitCounter2(void) {
    int value;

    value = 1;
    g_PlayTimeSeconds = 0;
    D_800A76C0 = 0;
    D_800A76B8 = value;
}

void GameTime_InitCounter1(void) {
    int value;

    value = 1;
    D_800A76B0 = 0;
    D_800A76B4 = 0;
    D_800A76AC = value;
}
