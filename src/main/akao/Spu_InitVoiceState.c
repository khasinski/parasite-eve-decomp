typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

#define U16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define U32(base, off) (*(u32 *)((u8 *)(base) + (off)))
#define G9_U16(off) (*(u16 *)((u8 *)&D_8009D200 + (off)))
#define G9_U32(off) (*(u32 *)((u8 *)&D_8009D200 + (off)))
#define GBCD_U32(off) (*(u32 *)((u8 *)&D_800BCD50 + (off)))

extern u32 D_8009D200;
extern u32 D_800BCD50;
extern u8 D_800B6980[];
extern u8 D_800B8AC0[];
extern u8 D_800BC03C[];
extern u8 D_800C0D90[];
extern u32 D_800B8628;

void SPU_WriteVoiceRegs(void);
void Spu_SetVoiceAttr(u32 voice, u32 arg1, u32 arg2, u32 arg3, u32 arg4);
void Seq_SetParamWithReset(u32 param);
void SPU_ReadRegister(u32 reg);

void Spu_InitVoiceState(void) {
    u32 i;
    u8 *voice;
    u8 *track;
    u8 *state;

    G9_U32(0xC8) = (u32)D_800B6980;
    U32(D_800B6980, 0xB0) = 0x7F0000;
    U32(D_800B6980, 0x48) = 0x7F0000;
    G9_U32(0xB4) = 0x7FFF0000;
    G9_U32(0xC0) = 1;
    GBCD_U32(0x00) = 0;

    U32(D_800B6980, 0x04) = 0;
    U32(D_800B6980, 0x08) = 0;
    U16(D_800B6980, 0x54) = 0;
    GBCD_U32(0x10) = 0;
    U32(D_800B6980, 0x1C) = 0;
    U32(D_800B6980, 0x84) = 0;
    U32(D_800B6980, 0x6C) = 0;
    U32(D_800B6980, 0x70) = 0;
    U16(D_800B6980, 0xBC) = 0;
    U16(D_800B6980, 0xB8) = 0;
    U16(D_800B6980, 0x50) = 0;

    G9_U16(0x1E) = 0;
    G9_U32(0xCC) = 0;
    G9_U16(0x20) = 0;
    G9_U32(0xD0) = 0;
    G9_U16(0xA2) = 0;
    GBCD_U32(0x1C) = 0;
    U32(D_800B6980, 0x34) = 0;
    GBCD_U32(0x20) = 0;
    U32(D_800B6980, 0x38) = 0;
    GBCD_U32(0x24) = 0;
    U32(D_800B6980, 0x3C) = 0;
    U32(D_800B6980, 0xA4) = 0;
    U32(D_800B6980, 0xA0) = 0;
    U32(D_800B6980, 0x9C) = 0;
    D_800B8628 = 0;
    U16(D_800B6980, 0x60) = 0;
    U16(D_800B6980, 0x5E) = 0;
    U16(D_800B6980, 0x5C) = 0;
    U16(D_800B6980, 0x64) = 0;

    U32(D_800C0D90, 0x00) = 0x3FCF;
    U16(D_800C0D90, 0x06) = 0x3FFF;
    U16(D_800C0D90, 0x04) = 0x3FFF;
    U16(D_800C0D90, 0x08) = 0;
    U16(D_800C0D90, 0x0A) = 0;
    U16(D_800C0D90, 0x12) = 0x7FFF;
    U16(D_800C0D90, 0x10) = 0x7FFF;
    U32(D_800C0D90, 0x14) = 0;
    U32(D_800C0D90, 0x18) = 1;
    U16(D_800C0D90, 0x1E) = 0;
    U16(D_800C0D90, 0x1C) = 0;
    U32(D_800C0D90, 0x20) = 0;
    U32(D_800C0D90, 0x24) = 0;
    SPU_WriteVoiceRegs();

    G9_U32(0x68) = 0;
    for (i = 0; i < 24; i++) {
        voice = D_800B8AC0 + i * 0x11C;
        U32(voice, 0x38) = 0;
        U32(voice, 0xF0) = 24;
        U16(voice, 0x54) = 0;
        U32(voice, 0x50) = 0;
        Spu_SetVoiceAttr(i, 0, 0, 0, 0);
    }

    for (i = 0; i < 24; i++) {
        voice = D_800B8AC0 + (24 * 0x11C) + i * 0x11C;
        U32(voice, 0x38) = 0;
        U32(voice, 0xF0) = 24;
        U16(voice, 0x54) = 0;
        U32(voice, 0x50) = 0;
        Spu_SetVoiceAttr(i, 0, 0, 0, 0);
    }

    for (i = 12, track = D_800BC03C; i < 24; i++, track += 0x11C) {
        U32(track, -4) = 0;
        U32(track, 0xB4) = i;
        U16(track, 0x18) = 1;
        U32(track, 0x14) = 0;
        U16(track, 0x9C) = 0x7F00;
        U16(track, 0x38) = 0;
        U16(track, 0x34) = 0;
        U32(track, 0x00) = 0;
    }

    state = (u8 *)G9_U32(0xC8);
    U32(state, 0x18) = 0;
    U32(state, 0x14) = 0;
    U32(state, 0x10) = 0;
    U32(state, 0x80) = 0;
    U32(state, 0x7C) = 0;
    U32(state, 0x78) = 0;
    GBCD_U32(0x18) = 1;
    GBCD_U32(0x14) = 0x66A80000;
    GBCD_U32(0x0C) = 0;
    GBCD_U32(0x08) = 0;
    GBCD_U32(0x04) = 0;
    U32(state, 0xA8) = 0x3FFF0000;
    U32(state, 0x40) = 0x3FFF0000;
    U32(state, 0xAC) = 0;
    U32(state, 0x44) = 0;
    U16(state, 0xC0) = 0;
    U16(state, 0x58) = 0;

    G9_U32(0xC4) |= 0x80;
    Seq_SetParamWithReset(4);
    SPU_ReadRegister(1);
}
