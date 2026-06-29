/* CC1_PROFILE: build0001 */

extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Akao_InitVoices(int, void *);

void Akao_InitPrimarySecondaryVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    Akao_InitVoices(0, base);
    g_AkaoCurTrack += 0x68;
    Akao_InitVoices(0, base + 0x1AA0);
    g_AkaoCurTrack -= 0x68;
}
