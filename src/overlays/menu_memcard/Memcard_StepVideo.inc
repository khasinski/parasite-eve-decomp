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

/* Wait for the next decoded frame and start its decode; -1 on a stall. */
static inline s16 Memcard_NextVideoFrame(VideoDisplay *dec) {
    s16 retries = 2000;
    void *frame;

    while ((frame = MEMCARD_VIDEO_UPDATE_FRAME(dec)) == 0) {
        if (--retries == 0) {
            return -1;
        }
    }
    D_800B0DBC.value++;
    dec->decodeIndex ^= 1;
    func_8010C89C(frame, dec->decode[dec->decodeIndex], MEMCARD_VIDEO_PTR4);
    func_8007C394(frame);
    return 0;
}

/* Restart the stream at the saved position. */
static inline void Memcard_KickVideoStream(CdlLOC *loc) {
    u8 result[8];

    func_8007C2A0(loc);
    do {
        do {
            while (func_8007F72C() != 1) {
            }
        } while (CdRom_GetPendingReadCount() != 0);
        func_80080D5C(2, loc, result);
    } while (!func_80081314(loc, 0x1E0));
}

/* Wait for the slice upload; on a timeout skip to the other frame buffer. */
static inline void Memcard_SyncVideoUpload(VideoDisplay *dec) {
    volatile s32 wait = 0x800000;

    while (dec->done == 0) {
        if (--wait == 0) {
            dec->done = 1;
            dec->region ^= 1;
            dec->rect.x = dec->regions[dec->region].x;
            dec->rect.y = dec->regions[dec->region].y;
        }
    }
    dec->done = 0;
}

/* Per-frame step: start the decode and the slice upload, wait for the next
 * frame (restarting the stream on a stall), wait for the upload, then stop
 * the player once the last frame has been shown. */
s32 MEMCARD_STEP_VIDEO_NAME(void) {
    s8 finished;
    u8 *flip;

    if (D_800B0DBA >= 2) {
        flip = &MEMCARD_VIDEO_FLIP;
        finished = 0;
        if (*flip == 2) {
            MEMCARD_VIDEO_SET_DISPLAY(D_8009CDDC ^ 1, D_800B0DBB);
            *flip = 0;
        }
        MEMCARD_VIDEO_RESTART = MEMCARD_VIDEO_FILE.pos;
        func_8010BFA0(MEMCARD_VIDEO_DISPLAY.decode[MEMCARD_VIDEO_DISPLAY.decodeIndex], MEMCARD_VIDEO_MODE);
        func_8010C01C(MEMCARD_VIDEO_DISPLAY.buffers[MEMCARD_VIDEO_DISPLAY.selector],
                      MEMCARD_VIDEO_DISPLAY.rect.w * MEMCARD_VIDEO_DISPLAY.rect.h / 2);
        while (Memcard_NextVideoFrame(&MEMCARD_VIDEO_DISPLAY) == -1) {
            Memcard_KickVideoStream(&MEMCARD_VIDEO_RESTART);
        }
        Memcard_SyncVideoUpload(&MEMCARD_VIDEO_DISPLAY);
        if (MEMCARD_VIDEO_FINISHED == 1) {
            finished = 1;
        }
        if (!finished) {
            return 1;
        }
        {
            u8 *active = &D_800B0DBA;

            (*active)--;
        }
        func_8010C0D8(0);
        func_8007A2A4();
        func_80080DC4(9, 0, 0);
    }
    return 0;
}
