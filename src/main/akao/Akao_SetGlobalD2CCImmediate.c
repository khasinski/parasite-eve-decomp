/* CC1_PROFILE: build0001 */
typedef signed char s8;

typedef struct {
    int unk0;
    s8 value;
} AkaoGlobalParamCommand;

extern short g_AkaoGlobalD2CCSlideCounter;
extern int D_8009D2CC;

void Akao_SetGlobalD2CCImmediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2CCSlideCounter = 0;
    D_8009D2CC = value << 16;
}
