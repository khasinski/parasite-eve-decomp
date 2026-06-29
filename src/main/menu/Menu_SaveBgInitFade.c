/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeTint;

void Menu_ComputeGammaLut(int arg0, int arg1);

void Menu_SaveBgInitFade(void) {
    g_MenuSaveBgFadeState = 0;
    g_MenuSaveBgFadeLutLen = 0;
    g_MenuSaveBgFadeStep = 0;
    g_MenuSaveBgFadeHeight = 0x20;
    Menu_ComputeGammaLut(0x90, 0xFF);
    g_MenuSaveBgFadeTint = 0x48;
}
