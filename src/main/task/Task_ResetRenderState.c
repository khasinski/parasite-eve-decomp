void Pm_StopAllBoth(void);

int Task_ResetRenderState(void) {
    Pm_StopAllBoth();
    return 1;
}
