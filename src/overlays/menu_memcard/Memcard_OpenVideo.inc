#ifndef MEMCARD_OPEN_VIDEO_NAME
#define MEMCARD_OPEN_VIDEO_NAME Memcard_OpenVideo
#define MEMCARD_VIDEO_TABLE D_801D0E00
#define MEMCARD_VIDEO_ENTRY D_801D11AC
#define MEMCARD_VIDEO_SET_DISPLAY Memcard_SetVideoDisplay
#define MEMCARD_VIDEO_DIRECTORY D_8018F2E4
#define MEMCARD_VIDEO_DIRECTORY_LATE D_8018F2EC
#define MEMCARD_VIDEO_FILE D_801D0DC4
#define MEMCARD_VIDEO_RESTART D_801D0DDC
#define MEMCARD_VIDEO_DISPLAY D_801D1464
#define MEMCARD_VIDEO_PTR0 D_801D0DE8
#define MEMCARD_VIDEO_PTR1 D_801D0DEC
#define MEMCARD_VIDEO_PTR2 D_801D0DF0
#define MEMCARD_VIDEO_PTR3 D_801D0DF4
#define MEMCARD_VIDEO_PTR4 D_801D0DF8
#define MEMCARD_VIDEO_PTR5 D_801D0DFC
#define MEMCARD_VIDEO_UPLOAD Memcard_UploadVideoSlice
#define MEMCARD_VIDEO_UPDATE_FRAME Memcard_UpdateVideoFrame
#define MEMCARD_VIDEO_FINISHED D_801D0DBD
#endif

/* Wait for the next decoded frame and start its decode; -1 on a stall. */
static inline s16 Memcard_OpenNextVideoFrame(VideoDisplay *dec) {
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

/* Restart reading at a stream position until the drive accepts it. */
static inline void Memcard_OpenKickVideoStream(CdlLOC *loc, u8 *result) {
    do {
        do {
            while (func_8007F72C() != 1) {
            }
        } while (CdRom_GetPendingReadCount() != 0);
        func_80080D5C(2, loc, result);
    } while (!func_80081314(loc, 0x1E0));
}

/* Start reading at the file position, retrying from the drive wait. */
static inline void Memcard_OpenStartVideoStream(CdlLOC *loc, u8 *result) {
    /* Recorded crutch debt: goto restart of the stream start, not
     * reproducible with a structured loop in stock GCC 2.7.2 (loop.c would
     * hoist the per-try constant 1 out of the restart loop). */
start:
    do {
        while (func_8007F72C() != 1) {
        }
    } while (CdRom_GetPendingReadCount() != 0);
    func_80080D5C(2, loc, result);
    if (!func_80081314(loc, 0x1E0)) goto start;
}

/* Reset the decode environment: buffers, both display areas, first slice. */
static inline void Memcard_OpenSetDecodeEnv(VideoDisplay *dec, s16 x0, s16 y0, s16 x1, s16 y1) {
    dec->decode[0] = MEMCARD_VIDEO_PTR0;
    dec->decode[1] = MEMCARD_VIDEO_PTR1;
    dec->decodeIndex = 0;
    dec->buffers[0] = MEMCARD_VIDEO_PTR2;
    dec->buffers[1] = MEMCARD_VIDEO_PTR3;
    dec->selector = 0;
    dec->regions[0].x = x0;
    dec->regions[0].y = y0;
    dec->regions[1].x = x1;
    dec->regions[1].y = y1;
    dec->region = D_8009CDDC;
    dec->rect.x = dec->regions[dec->region].x;
    dec->rect.y = dec->regions[dec->region].y;
    if (D_800B0DBB) {
        dec->rect.w = 24;
    } else {
        dec->rect.w = 16;
    }
    dec->done = 0;
}

/* Look up the video file, set up the decoder state, start the stream and
 * decode the first frame. */
s32 MEMCARD_OPEN_VIDEO_NAME(s32 number) {
    s16 index = number;
    VideoRect size;
    char name[32];
    u8 result[8];
    VideoEntry *entry;
    u32 found;

    if ((u16)number >= 47) {
        return 0;
    }
    D_800B0DBF = index;
    MEMCARD_VIDEO_ENTRY = &MEMCARD_VIDEO_TABLE[index];
    D_800B0DBB = MEMCARD_VIDEO_ENTRY->wide;
    MEMCARD_VIDEO_SET_DISPLAY(0, D_800B0DBB);
    MEMCARD_VIDEO_SET_DISPLAY(1, D_800B0DBB);
    name[0] = 0;
    if ((u16)number < 21) {
        func_800719F4(name, MEMCARD_VIDEO_DIRECTORY);
    } else {
        func_800719F4(name, MEMCARD_VIDEO_DIRECTORY_LATE);
    }
    func_800719F4(name, MEMCARD_VIDEO_ENTRY->name);
    /* Recorded crutch debt: retail's CD retry loop is a goto restart, not
     * reproducible with a structured loop in stock GCC 2.7.2 (loop.c
     * hoists the per-try constants out of any loop with loop notes). */
search:
    found = 0;
    if (func_8007F72C() == 1 && CdRom_GetPendingReadCount() == 0) {
        found = func_80081414(&MEMCARD_VIDEO_FILE, name);
    }
    if (found == 0 || found == -1) goto search;
    MEMCARD_VIDEO_RESTART = MEMCARD_VIDEO_FILE.pos;
    entry = MEMCARD_VIDEO_ENTRY;
    size.x = entry->x;
    size.y = entry->y;
    Memcard_OpenSetDecodeEnv(&MEMCARD_VIDEO_DISPLAY, size.x, size.y + 240, size.x, size.y);
    func_8010BE3C(0);
    func_8010C0D8(MEMCARD_VIDEO_UPLOAD);
    func_8007A214(MEMCARD_VIDEO_PTR5, 0x40);
    func_8007C304(1, MEMCARD_VIDEO_ENTRY->speed, -1, 0, 0);
    Memcard_OpenStartVideoStream(&MEMCARD_VIDEO_FILE.pos, result);
    func_800870F0(D_800B0DBE);
    func_8010BD4C(MEMCARD_VIDEO_PTR4);
    while (Memcard_OpenNextVideoFrame(&MEMCARD_VIDEO_DISPLAY) == -1) {
        MEMCARD_VIDEO_RESTART = MEMCARD_VIDEO_FILE.pos;
        Memcard_OpenKickVideoStream(&MEMCARD_VIDEO_RESTART, result);
    }
    MEMCARD_VIDEO_FINISHED = 0;
    D_800B0DBC.value = 1;
    {
        u8 *active = &D_800B0DBA;

        (*active)++;
    }
    return 1;
}
