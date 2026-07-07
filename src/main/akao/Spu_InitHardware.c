/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
s32 EnableEvent(s32);
s32 OpenEvent(s32, s32, s32, void *);
s32 SetRCnt(s32, s32, s32);
void SpuInitMalloc(s32, void *);
void SpuSetIRQ(s32);
void SpuStart(void);
void Spu_UploadWithPrepare(void *, s32);
s32 StartRCnt(s32);
void Spu_WaitTransferDone(void);
void Spu_InitVoiceState(void);
void Spu_SetTransferMode(s32);
void Spu_WriteRegChecked(s32);
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
