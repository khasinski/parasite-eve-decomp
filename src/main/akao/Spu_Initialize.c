#include "pe1/akao/init_state.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao.h"
#include "pe1/psyq_spu_internal.h"
#define U16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define U32(base, off) (*(u32 *)((u8 *)(base) + (off)))
#define G9_U16(off) (*(u16 *)((u8 *)&D_8009D200 + (off)))
#define G9_U32(off) (*(u32 *)((u8 *)&D_8009D200 + (off)))
#define GBCD_U32(off) (*(u32 *)((u8 *)&D_800BCD50 + (off)))

void Spu_InitVoiceState(void) {
    u16 i;
    u8 *voice;
    u8 *voice_base;
    u32 voice_count;
    u8 *track;
    u32 track_enabled;
    u32 track_volume;
    u32 initial_flags;
    u8 *state;

    G9_U32(0xC8) = (u32)D_800B6980;
    G9_U32(0xC0) = 1;
    U32(D_800B6980, 0xB0) = 0x7F0000;
    U32(D_800B6980, 0x48) = 0x7F0000;
    G9_U32(0xB4) = 0x7FFF0000;
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
    *(u32 *)&D_800B8628[0] = 0;
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
    SpuSetCommonAttr((SpuCommonSettings *)D_800C0D90);

    i = 0;
    voice_base = D_800B8AC0;
    voice_count = 24;
    G9_U32(0x68) = 0;
    G9_U32(0x2C) = 0;
    G9_U32(0xB8) = 0;
    U32(D_800C0D90, 0x48) = 0;
    U32(D_800C0D90, 0x44) = 0;
    U32(D_800C0D90, 0x40) = 0;
    /* The original reloads the reset mask before copying it to three slots. */
    initial_flags = *(volatile u32 *)((u8 *)&D_8009D200 + 0x68);
    G9_U32(0xF4) = initial_flags;
    G9_U32(0xDC) = initial_flags;
    G9_U32(0xE0) = initial_flags;
    voice = voice_base;
    for (; i < 24; i++, voice += 0x11C, voice_base += 0x11C) {
        U32(voice, 0x38) = 0;
        U32(voice, 0xF0) = voice_count;
        U16(voice, 0x54) = 0;
        U32(voice, 0x50) = 0;
        Spu_SetVoiceAttr(i, 0, 0, 0, 0);
    }

    i = 0;
    voice_count = 24;
    voice = voice_base;
    for (; i < 24; i++) {
        U32(voice, 0x38) = 0;
        U32(voice, 0xF0) = voice_count;
        U16(voice, 0x54) = 0;
        U32(voice, 0x50) = 0;
        Spu_SetVoiceAttr(i, 0, 0, 0, 0);
        voice += 0x11C;
    }

    i = 12;
    track_enabled = 1;
    track_volume = 0x7F00;
    track = (u8 *)g_AkaoVoiceChannelTable;
    for (; i < 24; i++, track += 0x11C) {
        U32(track, 0x38) = 0;
        U32(track, 0xF0) = i;
        U16(track, 0x54) = track_enabled;
        U32(track, 0x50) = 0;
        U16(track, 0xD8) = track_volume;
        U16(track, 0x74) = 0;
        U16(track, 0x70) = 0;
        U32(track, 0x3C) = 0;
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
    SpuSetReverb(1);
}

s32 EnableEvent(s32);
s32 OpenEvent(s32, s32, s32, void *);
s32 SetRCnt(s32, s32, s32);
void SpuInitMalloc(s32, void *);
void SpuSetIRQ(s32);
s32 StartRCnt(s32);
void Spu_WaitTransferDone(void);
void Spu_SetReverbMode(s32);
extern u8 D_8009B7FC[];
#define D_8009B7FC (D_8009B7FC[0])
extern s32 g_AkaoTimerEventDesc[];
#define g_AkaoTimerEventDesc (g_AkaoTimerEventDesc[0])
extern u8 D_800B6958[];
#define D_800B6958 (D_800B6958[0])
extern u8 Akao_TimerCallback[];
#define Akao_TimerCallback (Akao_TimerCallback[0])

void Spu_InitHardware(void) {
    s32 eventDesc;

    SpuStart();
    SpuInitMalloc(4, &D_800B6958);
    Spu_SetReverbMode(0);
    Spu_WriteRegChecked(0x1010);
    Spu_UploadWithPrepare(&D_8009B7FC, 0x20);
    Spu_WaitTransferDone();
    Spu_InitVoiceState();
    SpuSetIRQ(0);
    Spu_SetTransferMode(0);
    do {

    } while (SetRCnt(0xF2000002, 0x44E8, 0x1000) == 0);
    do {

    } while (StartRCnt(0xF2000002) == 0);
    do {
        eventDesc = OpenEvent(0xF2000002, 2, 0x1000, &Akao_TimerCallback);
        g_AkaoTimerEventDesc = eventDesc;
    } while (eventDesc == -1);
    do {

    } while (EnableEvent(g_AkaoTimerEventDesc) == 0);
}
