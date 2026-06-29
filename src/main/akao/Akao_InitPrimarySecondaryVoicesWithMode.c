/* CC1_PROFILE: build0001 */

typedef struct {
    int pad_0;
    int field_4;
} AkaoCommand8008C3E4;

extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Akao_InitVoices(int, void *);

void Akao_InitPrimarySecondaryVoicesWithMode(AkaoCommand8008C3E4 *cmd) {
    char *base = g_AkaoVoiceStateTable;
    int value = cmd->field_4;

    Akao_InitVoices(value, base);
    if (cmd->field_4 != 0) {
        g_AkaoCurTrack += 0x68;
        Akao_InitVoices(cmd->field_4, base + 0x1AA0);
        g_AkaoCurTrack -= 0x68;
    }
}
