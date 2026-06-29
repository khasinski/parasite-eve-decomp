extern volatile int D_800A76AC;
extern volatile int D_800A76B0;
extern volatile int D_800A76B4;

void GameTime_InitCounter1(void) {
    int value;

    value = 1;
    D_800A76B0 = 0;
    D_800A76B4 = 0;
    D_800A76AC = value;
}
