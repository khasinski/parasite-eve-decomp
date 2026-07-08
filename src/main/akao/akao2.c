/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

extern short g_AkaoGlobalD2CCSlideCounter;
extern int g_AkaoGlobalD2CCSlideStep;
extern int D_8009D2CC;

extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Akao_InitVoices(int arg0, char *arg1);

extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuPendingKeyOffMask;
extern unsigned int g_AkaoVoiceUpdateFlags;
extern char g_AkaoVoiceChannelTable[];

void SeqOp_DeactivateVoice(char *ptr, int mask);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Akao_SlideGlobalD2CCToTarget(AkaoGlobalSlideCommand *cmd) {
    int raw_duration = cmd->duration;
    int duration = 1;
    int step;

    if (raw_duration != 0) {
        duration = raw_duration;
    }

    step = ((cmd->target << 16) - D_8009D2CC) / duration;
    g_AkaoGlobalD2CCSlideCounter = duration;
    g_AkaoGlobalD2CCSlideStep = step;
}

void Akao_SlideGlobalD2CCFromStartToTarget(AkaoGlobalSlideRangeCommand *cmd) {
    int duration;
    int start = cmd->start;
    int step;

    if (start == 0) {
        duration = 1;
    } else {
        duration = cmd->duration;
    }

    start <<= 24;
    start >>= 8;
    D_8009D2CC = start;
    step = ((cmd->target << 16) - start) / duration;
    g_AkaoGlobalD2CCSlideCounter = duration;
    g_AkaoGlobalD2CCSlideStep = step;
}

void Akao_InitPrimarySecondaryVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    Akao_InitVoices(0, base);
    g_AkaoCurTrack += 0x68;
    Akao_InitVoices(0, base + 0x1AA0);
    g_AkaoCurTrack -= 0x68;
}

void Akao_InitPrimarySecondaryVoicesWithMode(AkaoValueCommand *cmd) {
    char *base = g_AkaoVoiceStateTable;
    int value = cmd->field_4;

    Akao_InitVoices(value, base);
    if (cmd->field_4 != 0) {
        g_AkaoCurTrack += 0x68;
        Akao_InitVoices(cmd->field_4, base + 0x1AA0);
        g_AkaoCurTrack -= 0x68;
    }
}

void Spu_StopActiveVoices(void) {
    char *voice = g_AkaoVoiceChannelTable;
    unsigned int mask = AKAO_SPU_VOICE_SFX_START_MASK;
    unsigned int i = 0;
    unsigned int skip_flag = 0x02000000;
    char *field_38 = voice + 0x38;

    do {
        if (g_SpuActiveVoiceMask & mask) {
            if ((*(int *)(field_38 - 0xC) & skip_flag) == 0) {
                g_SpuPendingKeyOffMask |= mask;
                SeqOp_DeactivateVoice(voice, mask);
                *(int *)field_38 = 0;
            }
        }
        i++;
        field_38 += sizeof(AkaoTrack);
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 0xC);

    g_AkaoVoiceUpdateFlags |= AKAO_VOICE_PARAM_PITCH;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
