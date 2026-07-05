/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct SpuVolumeState {
    u32 unk0;
    u16 unk4;
    u16 unk6;
    u32 unk8;
    u32 unkC;
} SpuVolumeState;

void ResetCallback(void);
void _spu_init(int hot);
void SpuStart(void);
void _spu_FsetRXX(u32 offset, u32 value, u32 mode);

extern u16 D_8009B3E6;
extern SpuVolumeState D_8009B3A0;
extern int D_8009B38C;
extern int D_8009B388;
extern int g_SpuCdAudioMixMode;
extern int g_SpuCdAudioMixModePrev;
extern int g_SpuCdInputVolume;
extern int D_8009B3B0;
extern int D_8009B3B4;
extern int D_8009B3E8;
extern int D_8009B418;
extern int _spu_AllocBlockNum;
extern int _spu_AllocLastNum;
extern int _spu_memList;
extern int g_SpuCdInputVolumeTable;

void _SpuInit(int hot) {
    int i;
    int cdVolume;
    int reset;
    register int regAddr asm("$4");
    u16 *vol;
    SpuVolumeState *state;

    ResetCallback();
    _spu_init(hot);
    reset = 0xC000;
    if (hot == 0) {
        i = 0x17;
        vol = &D_8009B3E6;
        do {
            *vol = reset;
            i--;
            vol--;
        } while (i >= 0);
    }
    SpuStart();
    regAddr = 0xD1;
    state = &D_8009B3A0;
    asm volatile("" : "=r"(state) : "0"(state));
    cdVolume = g_SpuCdInputVolumeTable;
    g_SpuCdAudioMixMode = 0;
    g_SpuCdAudioMixModePrev = 0;
    state->unk0 = 0;
    state->unk4 = 0;
    state->unk6 = 0;
    state->unk8 = 0;
    state->unkC = 0;
    g_SpuCdInputVolume = cdVolume;
    _spu_FsetRXX(regAddr, cdVolume, 0);
    _spu_AllocBlockNum = 0;
    _spu_AllocLastNum = 0;
    _spu_memList = 0;
    D_8009B38C = 0;
    D_8009B418 = 0;
    D_8009B388 = 0;
    D_8009B3B4 = 0;
    D_8009B3B0 = 0;
    D_8009B3E8 = 0;
}
