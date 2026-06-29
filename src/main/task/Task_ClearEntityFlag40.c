extern unsigned char g_ScreenTransitionState;

int Task_ClearEntityFlag40(void) {
    unsigned char *ptr = &g_ScreenTransitionState;

    *ptr &= 0xBF;
    return 1;
}
