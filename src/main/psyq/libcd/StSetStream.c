extern int D_800C0DB8;
extern int g_StrDataReadyCallback;
extern int D_800A801C;
extern int D_800B8620;
extern int D_800B6914;
extern short D_800A8018;
extern int D_800A5D54;
extern int D_800B0CCC;

void StSetMask(int mask, int start, int end);

void StSetStream(int mode, int start_frame, int end_frame, int func1, int func2) {
    StSetMask(1, start_frame, end_frame);

    D_800C0DB8 = 0;
    g_StrDataReadyCallback = func1;
    D_800A801C = mode & 1;
    D_800B8620 = 0;
    D_800B6914 = 0;
    D_800A8018 = 0;
    D_800A5D54 = 0;
    D_800B0CCC = func2;
}
