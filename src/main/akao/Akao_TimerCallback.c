typedef int s32;

s32 GetRCnt(s32 arg0);
void Akao_Tick(void);

extern struct { char _[16]; } D_8009B7EC_o __asm__("D_8009B7EC");
#define D_8009B7EC (*(s32 *)&D_8009B7EC_o)
extern struct { char _[16]; } D_8009B7F0_load_o __asm__("g_AkaoTimerDeltaHist2");
#define D_8009B7F0_LOAD (*(s32 *)&D_8009B7F0_load_o)
extern struct { char _[16]; } D_8009B7F0_store_o __asm__("g_AkaoTimerDeltaHist2");
#define D_8009B7F0_STORE (*(s32 *)&D_8009B7F0_store_o)
extern struct { char _[16]; } D_8009B7F4_load_o __asm__("g_AkaoTimerDeltaHist1");
#define D_8009B7F4_LOAD (*(s32 *)&D_8009B7F4_load_o)
extern struct { char _[16]; } D_8009B7F4_store_o __asm__("g_AkaoTimerDeltaHist1");
#define D_8009B7F4_STORE (*(s32 *)&D_8009B7F4_store_o)
extern struct { char _[16]; } D_8009B7F8_load_o __asm__("g_AkaoTimerDeltaHist0");
#define D_8009B7F8_LOAD (*(s32 *)&D_8009B7F8_load_o)
extern struct { char _[16]; } D_8009B7F8_store_o __asm__("g_AkaoTimerDeltaHist0");
#define D_8009B7F8_STORE (*(s32 *)&D_8009B7F8_store_o)
extern struct { char _[16]; } D_8009CDE4_o __asm__("D_8009CDE4");
#define D_8009CDE4 (*(s32 *)&D_8009CDE4_o)

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
    old_f0 = D_8009B7F0_LOAD;
    old_f4 = D_8009B7F4_LOAD;
    old_f8 = D_8009B7F8_LOAD;
    value = delta;
    D_8009B7F8_STORE = value;
    D_8009B7EC = old_f0;
    old_f0 = old_f0 + old_f4;
    old_f0 = old_f0 + old_f8;
    old_f0 = old_f0 + value;

    D_8009B7F0_STORE = old_f4;
    D_8009B7F4_STORE = old_f8;
    D_8009CDE4 = old_f0;
}
