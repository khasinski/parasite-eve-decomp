/* MASPSX_FLAGS: --expand-div */

typedef unsigned short u16;

extern short g_AkaoGlobalPitchSlideCounter;
extern int g_AkaoGlobalPitchSlideStep;
extern int D_8009D2B4;

void Seq_SlideGlobalPitchFromStartToTarget(void *arg0) {
    int duration;
    int raw_duration;
    int start;
    int target;

    raw_duration = *(int *)((char *)arg0 + 4);
    duration = 1;
    if (raw_duration != 0) {
        duration = raw_duration;
    }

    target = *(u16 *)((char *)arg0 + 0xC) << 16;
    start = *(u16 *)((char *)arg0 + 8) << 16;
    g_AkaoGlobalPitchSlideCounter = duration;
    D_8009D2B4 = start;
    g_AkaoGlobalPitchSlideStep = (target - start) / duration;
}
