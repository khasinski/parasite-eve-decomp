/* CC1_PROFILE: build0001 */
typedef signed char s8;

typedef struct {
    int unk0;
    s8 value;
} AkaoGlobalParamCommand;

extern short g_AkaoGlobalD2D0SlideCounter;
extern int D_8009D2D0;

void Akao_SetGlobalD2D0Immediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2D0SlideCounter = 0;
    D_8009D2D0 = value << 16;
}
