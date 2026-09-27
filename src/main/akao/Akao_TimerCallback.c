#include "common.h"
s32 GetRCnt(s32 arg0);
void Akao_Tick(void);

extern s32 D_8009B7EC;
extern s32 g_AkaoTimerDeltaHist2;
extern s32 g_AkaoTimerDeltaHist1;
extern s32 g_AkaoTimerDeltaHist0;
extern s32 D_8009CDE4;

void Akao_TimerCallback(void) {
    s32 delta;
    s32 old_f0;
    s32 old_f4;
    s32 old_f8;
    register s32 value asm("$2");

    delta = GetRCnt(0xF2000002);
    Akao_Tick();
    delta = GetRCnt(0xF2000002) - delta;
    if (delta <= 0) {
        delta += 0x44E8;
    }
    old_f0 = g_AkaoTimerDeltaHist2;
    old_f4 = g_AkaoTimerDeltaHist1;
    old_f8 = g_AkaoTimerDeltaHist0;
    value = delta;
    g_AkaoTimerDeltaHist0 = value;
    D_8009B7EC = old_f0;
    old_f0 = old_f0 + old_f4;
    old_f0 = old_f0 + old_f8;
    old_f0 = old_f0 + value;

    g_AkaoTimerDeltaHist2 = old_f4;
    g_AkaoTimerDeltaHist1 = old_f8;
    D_8009CDE4 = old_f0;
}
