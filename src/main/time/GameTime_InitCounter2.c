extern volatile int D_800A76B8;
extern volatile int g_PlayTimeSeconds;
extern volatile int D_800A76C0;

void GameTime_InitCounter2(void) {
    int value;

    value = 1;
    g_PlayTimeSeconds = 0;
    D_800A76C0 = 0;
    D_800A76B8 = value;
}
