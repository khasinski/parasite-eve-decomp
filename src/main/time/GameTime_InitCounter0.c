extern volatile int D_800A76A0;
extern volatile int g_GameTimeTable;
extern volatile int D_800A76A8;

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
