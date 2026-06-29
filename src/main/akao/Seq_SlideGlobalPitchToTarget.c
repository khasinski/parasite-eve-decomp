/* MASPSX_FLAGS: --expand-div */

typedef unsigned short u16;

extern short g_AkaoGlobalPitchSlideCounter;
extern int g_AkaoGlobalPitchSlideStep;
extern int D_8009D2B4;

void Seq_SlideGlobalPitchToTarget(void *arg0) {
    int duration;
    int raw_duration;
    int value;

    raw_duration = *(int *)((char *)arg0 + 4);
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    value = *(u16 *)((char *)arg0 + 8) << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    g_AkaoGlobalPitchSlideStep = (value - D_8009D2B4) / duration;
}
