#ifndef MEMCARD_STEP_VIDEO_NAME
#define MEMCARD_STEP_VIDEO_NAME func_80192934
#define MEMCARD_VIDEO_STATE D_801D1464
#define MEMCARD_VIDEO_EVENT_FLAG D_801D0DC0
#define MEMCARD_VIDEO_DISPLAY_MODE D_801D0DBE
#define MEMCARD_VIDEO_START_LOC D_801D0DC4
#define MEMCARD_VIDEO_LOC D_801D0DDC
#define MEMCARD_VIDEO_FINISHED D_801D0DBD
#define MEMCARD_VIDEO_FRAME_TARGET D_801D0DF8
#define MEMCARD_VIDEO_SET_DISPLAY func_801918F8
#define MEMCARD_VIDEO_UPDATE_FRAME func_80191B64
#endif

s32 MEMCARD_STEP_VIDEO_NAME(void) {
    void *frame;
    s16 retries;
    s16 status;
    s32 finished;
    union {
        u8 result[8];
        volatile s32 timeout;
    } scratch;

    if (D_800B0DBA >= 2) {
    u8 *flag = &MEMCARD_VIDEO_EVENT_FLAG;

    finished = 0;
    if (*flag == 2) {
        MEMCARD_VIDEO_SET_DISPLAY(D_8009CDDC ^ 1, D_800B0DBB);
        *flag = 0;
    }
    MEMCARD_VIDEO_LOC = MEMCARD_VIDEO_START_LOC;
    func_8010BFA0(MEMCARD_VIDEO_STATE.decode[MEMCARD_VIDEO_STATE.decodeIndex], MEMCARD_VIDEO_DISPLAY_MODE);
    func_8010C01C(MEMCARD_VIDEO_STATE.buffers[MEMCARD_VIDEO_STATE.selector],
                  MEMCARD_VIDEO_STATE.rect.w * MEMCARD_VIDEO_STATE.rect.h / 2);
    retries = 2000;
retry:
    do {
        frame = MEMCARD_VIDEO_UPDATE_FRAME(&MEMCARD_VIDEO_STATE);
        if (frame != 0) {
            MEMCARD_VIDEO_STATE.decodeIndex ^= 1;
            D_800B0DBC++;
            func_8010C89C(frame, MEMCARD_VIDEO_STATE.decode[MEMCARD_VIDEO_STATE.decodeIndex],
                          MEMCARD_VIDEO_FRAME_TARGET);
            func_8007C394(frame);
            status = 0;
            goto check;
        }
    } while (--retries != 0);
    status = -1;
check:
    if (status == -1) {
        func_8007C2A0(&MEMCARD_VIDEO_LOC);
        do {
            while (func_8007F72C() != 1 || CdRom_GetPendingReadCount() != 0) {
            }
            func_80080D5C(2, &MEMCARD_VIDEO_LOC, scratch.result);
        } while (func_80081314(&MEMCARD_VIDEO_LOC, 480) == 0);
        retries = 2000;
        goto retry;
    }
    scratch.timeout = 0x800000;
    while (!MEMCARD_VIDEO_STATE.done) {
        if (--scratch.timeout == 0) {
            MEMCARD_VIDEO_STATE.done = 1;
            MEMCARD_VIDEO_STATE.region ^= 1;
            MEMCARD_VIDEO_STATE.rect.x = MEMCARD_VIDEO_STATE.regions[MEMCARD_VIDEO_STATE.region].x;
            MEMCARD_VIDEO_STATE.rect.y = MEMCARD_VIDEO_STATE.regions[MEMCARD_VIDEO_STATE.region].y;
        }
    }
    MEMCARD_VIDEO_STATE.done = 0;
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

#undef MEMCARD_VIDEO_UPDATE_FRAME
#undef MEMCARD_VIDEO_SET_DISPLAY
#undef MEMCARD_VIDEO_FRAME_TARGET
#undef MEMCARD_VIDEO_FINISHED
#undef MEMCARD_VIDEO_LOC
#undef MEMCARD_VIDEO_START_LOC
#undef MEMCARD_VIDEO_DISPLAY_MODE
#undef MEMCARD_VIDEO_EVENT_FLAG
#undef MEMCARD_VIDEO_STATE
#undef MEMCARD_STEP_VIDEO_NAME
