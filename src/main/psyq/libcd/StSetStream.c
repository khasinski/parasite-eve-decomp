/* MASPSX_FLAGS: --stack-return-delay */

extern int D_800C0DB8;
extern int g_StrDataReadyCallback;
extern int g_StStreamModeFlag;
extern int D_800B8620;
extern int D_800B6914;
extern short g_StStreamState;
extern int g_StStreamReadState;
extern int g_StStreamCallback2;

void StSetMask(int mask, int start, int end);

void StSetStream(int mode, int start_frame, int end_frame, int func1, int func2) {
    StSetMask(1, start_frame, end_frame);

    D_800C0DB8 = 0;
    g_StrDataReadyCallback = func1;
    g_StStreamModeFlag = mode & 1;
    D_800B8620 = 0;
    D_800B6914 = 0;
    g_StStreamState = 0;
    g_StStreamReadState = 0;
    g_StStreamCallback2 = func2;
}
