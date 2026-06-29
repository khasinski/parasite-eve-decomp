typedef unsigned short u16;

extern short g_AkaoGlobalPitchSlideCounter;
extern int D_8009D2B4;

void Seq_ApplyGlobalPitch(void);

void Seq_SetGlobalPitchImmediate(void *arg0) {
    int value;

    value = *(u16 *)((char *)arg0 + 4);
    g_AkaoGlobalPitchSlideCounter = 0;
    D_8009D2B4 = value << 16;
    Seq_ApplyGlobalPitch();
}
