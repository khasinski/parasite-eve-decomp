/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern unsigned int g_AkaoPendingBgmVolumeFade;

extern void Akao_Cmd_21(unsigned int arg0, int arg1);

void Akao_FlushBgmVolumeFade(void) {
    if (g_AkaoPendingBgmVolumeFade != 0) {
        Akao_Cmd_21(g_AkaoPendingBgmVolumeFade, 0);
        g_AkaoPendingBgmVolumeFade = 0;
    }
}
