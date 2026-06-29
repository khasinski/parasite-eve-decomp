/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 EnableEvent(s32);                               /* extern */
s32 OpenEvent(s32, M2C_UNK, M2C_UNK, M2C_UNK *);    /* extern */
s32 SetRCnt(s32, M2C_UNK, M2C_UNK);                 /* extern */
M2C_UNK SpuInitMalloc(M2C_UNK, M2C_UNK *);          /* extern */
M2C_UNK SpuSetIRQ(M2C_UNK);                         /* extern */
M2C_UNK SpuStart();                                 /* extern */
M2C_UNK Spu_UploadWithPrepare(M2C_UNK *, M2C_UNK);  /* extern */
s32 StartRCnt(s32);                                 /* extern */
M2C_UNK Spu_WaitTransferDone();                            /* extern */
M2C_UNK Spu_InitVoiceState();                            /* extern */
M2C_UNK Spu_SetTransferMode(M2C_UNK);                     /* extern */
M2C_UNK Spu_WriteRegChecked(M2C_UNK);                     /* extern */
M2C_UNK Spu_SetReverbMode(M2C_UNK);                     /* extern */
extern struct { char _[16]; } D_8009B7FC_o __asm__("D_8009B7FC");
#define D_8009B7FC (*(M2C_UNK *)&D_8009B7FC_o)
extern struct { char _[16]; } D_8009CDE0_o __asm__("g_AkaoTimerEventDesc");
#define g_AkaoTimerEventDesc (*(s32 *)&D_8009CDE0_o)
extern struct { char _[16]; } D_800B6958_o __asm__("D_800B6958");
#define D_800B6958 (*(M2C_UNK *)&D_800B6958_o)
extern struct { char _[16]; } func_8008E23C_o __asm__("Akao_TimerCallback");
#define Akao_TimerCallback (*(M2C_UNK *)&func_8008E23C_o)

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
