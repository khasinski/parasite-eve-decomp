void Pm_StopLowerHalf(void);
void Pm_StopUpperHalf(void);

void Pm_StopAllBoth(void) {
    Pm_StopLowerHalf();
    Pm_StopUpperHalf();
}
