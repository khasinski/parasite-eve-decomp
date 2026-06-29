/* CC1_PROFILE: build0001 */

typedef struct {
    int pad_0;
    int field_4;
} AkaoCommand8008C6D0;

extern int g_AkaoVoicePortamentoResetMask;
extern int g_AkaoTrackStateArray[];

void Seq_SetGlobalD2B8AndDirtyAllTracks(AkaoCommand8008C6D0 *cmd) {
    unsigned int i = 0;
    int value = cmd->field_4;
    int *slot;

    slot = g_AkaoTrackStateArray;
    g_AkaoVoicePortamentoResetMask = value;
    for (; i < 0x18; i++, slot = (int *)((char *)slot + 0x11C)) {
        *slot |= 3;
    }
}
