#ifndef MEMCARD_STEP_VIDEO_NAME
#define MEMCARD_STEP_VIDEO_NAME Memcard_StepVideo
#define MEMCARD_VIDEO_SET_DISPLAY Memcard_SetVideoDisplay
#define MEMCARD_VIDEO_UPDATE_FRAME Memcard_UpdateVideoFrame
#define MEMCARD_VIDEO_MODE D_801D0DBE
#define MEMCARD_VIDEO_FLIP D_801D0DC0
#define MEMCARD_VIDEO_FILE D_801D0DC4
#define MEMCARD_VIDEO_RESTART D_801D0DDC
#define MEMCARD_VIDEO_DISPLAY D_801D1464
#define MEMCARD_VIDEO_PTR4 D_801D0DF8
#define MEMCARD_VIDEO_FINISHED D_801D0DBD
#endif

s32 MEMCARD_STEP_VIDEO_NAME(void) {
    void *frame;
    s16 retries;
    s32 status;
    s32 finished;
    volatile s32 wait;

    if (D_800B0DBA >= 2) {
        u8 *flip = &MEMCARD_VIDEO_FLIP;

        finished = 0;
        if (*flip == 2) {
            MEMCARD_VIDEO_SET_DISPLAY(D_8009CDDC ^ 1, D_800B0DBB);
            *flip = 0;
        }
        MEMCARD_VIDEO_RESTART = MEMCARD_VIDEO_FILE.pos;
        func_8010BFA0(MEMCARD_VIDEO_DISPLAY.decode[MEMCARD_VIDEO_DISPLAY.decodeIndex], MEMCARD_VIDEO_MODE);
        func_8010C01C(MEMCARD_VIDEO_DISPLAY.buffers[MEMCARD_VIDEO_DISPLAY.selector],
                      MEMCARD_VIDEO_DISPLAY.rect.w * MEMCARD_VIDEO_DISPLAY.rect.h / 2);
        for (;;) {
            retries = 2000;
            do {
                frame = MEMCARD_VIDEO_UPDATE_FRAME(&MEMCARD_VIDEO_DISPLAY);
                if (frame) {
                    break;
                }
            } while (--retries);
            if (frame) {
                D_800B0DBC++;
                MEMCARD_VIDEO_DISPLAY.decodeIndex ^= 1;
                func_8010C89C(frame, MEMCARD_VIDEO_DISPLAY.decode[MEMCARD_VIDEO_DISPLAY.decodeIndex], MEMCARD_VIDEO_PTR4);
                func_8007C394(frame);
                status = 0;
            } else {
                status = -1;
            }
            if ((s16)status != -1) {
                break;
            }
            func_8007C2A0(&MEMCARD_VIDEO_RESTART);
            do {
                do {
                    while (func_8007F72C() != 1) {
                    }
                } while (CdRom_GetPendingReadCount() != 0
                         || (func_80080D5C(2, &MEMCARD_VIDEO_RESTART, &wait), 0));
            } while (!func_80081314(&MEMCARD_VIDEO_RESTART, 0x1E0));
        }
        wait = 0x800000;
        while (!MEMCARD_VIDEO_DISPLAY.done) {
            if (--wait == 0) {
                MEMCARD_VIDEO_DISPLAY.done = 1;
                MEMCARD_VIDEO_DISPLAY.region ^= 1;
                MEMCARD_VIDEO_DISPLAY.rect.x = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].x;
                MEMCARD_VIDEO_DISPLAY.rect.y = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].y;
            }
        }
        MEMCARD_VIDEO_DISPLAY.done = 0;
        if (MEMCARD_VIDEO_FINISHED == 1) {
            finished = 1;
        }
        if (!finished) {
            return 1;
        }
        D_800B0DBA--;
        func_8010C0D8(0);
        func_8007A2A4();
        func_80080DC4(9, 0, 0);
    }
    return 0;
}
