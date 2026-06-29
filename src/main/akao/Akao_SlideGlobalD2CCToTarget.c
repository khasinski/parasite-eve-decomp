/* CC1_PROFILE: build0001 */
/* MASPSX_FLAGS: --expand-div */
typedef signed char s8;

typedef struct {
    int unk0;
    int duration;
    s8 target;
} AkaoGlobalSlideCommand;

extern short g_AkaoGlobalD2CCSlideCounter;
extern int g_AkaoGlobalD2CCSlideStep;
extern int D_8009D2CC;

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
