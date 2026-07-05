/* MASPSX_FLAGS: --expand-div */

typedef unsigned int u32;
typedef unsigned char u8;

extern char D_800BC070[];
extern u32 g_SpuActiveVoiceMask;

typedef signed char s8;

typedef struct {
    int unk0;
    s8 value;
} AkaoGlobalParamCommand;

extern short g_AkaoGlobalD2D0SlideCounter;
extern int D_8009D2D0;

void Spu_SlideAllVoicePitch(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice asm("$6");
    int step;
    int delta;

    mask = 0x1000;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC070;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x44) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)((((u8 *)arg0)[8] << 8) - *(int *)(voice - 0x34));
                *(short *)voice = step;
                *(int *)(voice - 0x30) = (short)(delta / (short)step);
            }
        }
        i++;
        voice += 0x11C;
        mask <<= 1;
    } while (i < 12);
}

void Akao_SetGlobalD2D0Immediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2D0SlideCounter = 0;
    D_8009D2D0 = value << 16;
}
