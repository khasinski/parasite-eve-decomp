/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* g_GeomOtZ lives at 0x800BD024, outside the gp window; the incomplete
 * array type keeps -G8 from treating the 1-byte extern as small data. */
extern unsigned char g_GeomOtZ[];
extern int g_MenuSaveBgFadeState;
extern int g_MenuSaveBgFadeHeight;
extern int g_MenuSaveBgFadeStep;
extern int g_MenuSaveBgFadeIndex;

void MemCard_InitSlotState(void) {
    int value = g_GeomOtZ[0];

    g_MenuSaveBgFadeState = 1;
    g_MenuSaveBgFadeStep = 1;
    g_MenuSaveBgFadeIndex = 0;
    g_MenuSaveBgFadeHeight = value;

    /* value is a zero-extended byte, so the < 0 arm is dead at runtime,
     * but it is what retail compiled (bgez). */
    if (value < 0) {
        g_MenuSaveBgFadeHeight = 1;
    } else if (value >= 0x21) {
        g_MenuSaveBgFadeHeight = 0x20;
    }
}
