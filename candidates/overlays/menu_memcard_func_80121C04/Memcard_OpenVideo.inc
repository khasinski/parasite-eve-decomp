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

s32 MEMCARD_OPEN_VIDEO_NAME(s16 index) {
    VideoRect size;
    char name[32];
    u8 result[8];
    VideoEntry *entry;
    void *frame;
    s16 retries;
    VideoFile *found;
    s32 status;

    if ((u16)index >= 47) {
        return 0;
    }
    D_800B0DBF = index;
    MEMCARD_VIDEO_ENTRY = entry = &MEMCARD_VIDEO_TABLE[index];
    D_800B0DBB = entry->wide;
    MEMCARD_VIDEO_SET_DISPLAY(0, D_800B0DBB);
    MEMCARD_VIDEO_SET_DISPLAY(1, D_800B0DBB);
    name[0] = 0;
    if ((u16)index < 21) {
        func_800719F4(name, MEMCARD_VIDEO_DIRECTORY);
    } else {
        func_800719F4(name, MEMCARD_VIDEO_DIRECTORY_LATE);
    }
    func_800719F4(name, MEMCARD_VIDEO_ENTRY->name);
search:
    found = 0;
    if (func_8007F72C() == 1 && CdRom_GetPendingReadCount() == 0) {
        found = func_80081414(&MEMCARD_VIDEO_FILE, name);
    }
    if (found == 0 || found == (VideoFile *)-1) goto search;
    MEMCARD_VIDEO_RESTART = MEMCARD_VIDEO_FILE.pos;
    entry = MEMCARD_VIDEO_ENTRY;
    MEMCARD_VIDEO_DISPLAY.decode[0] = MEMCARD_VIDEO_PTR0;
    MEMCARD_VIDEO_DISPLAY.decode[1] = MEMCARD_VIDEO_PTR1;
    MEMCARD_VIDEO_DISPLAY.decodeIndex = 0;
    MEMCARD_VIDEO_DISPLAY.buffers[0] = MEMCARD_VIDEO_PTR2;
    MEMCARD_VIDEO_DISPLAY.buffers[1] = MEMCARD_VIDEO_PTR3;
    MEMCARD_VIDEO_DISPLAY.selector = 0;
    size.x = entry->x;
    size.y = entry->y;
    MEMCARD_VIDEO_DISPLAY.regions[0].x = size.x;
    MEMCARD_VIDEO_DISPLAY.regions[0].y = size.y + 240;
    MEMCARD_VIDEO_DISPLAY.regions[1].x = size.x;
    MEMCARD_VIDEO_DISPLAY.regions[1].y = size.y;
    MEMCARD_VIDEO_DISPLAY.region = D_8009CDDC;
    MEMCARD_VIDEO_DISPLAY.rect.x = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].x;
    MEMCARD_VIDEO_DISPLAY.rect.y = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].y;
    MEMCARD_VIDEO_DISPLAY.rect.w = D_800B0DBB ? 24 : 16;
    MEMCARD_VIDEO_DISPLAY.done = 0;
    func_8010BE3C(0);
    func_8010C0D8(MEMCARD_VIDEO_UPLOAD);
    func_8007A214(MEMCARD_VIDEO_PTR5, 0x40);
    func_8007C304(1, MEMCARD_VIDEO_ENTRY->speed, -1, 0, 0);
    for (;;) {
        while (func_8007F72C() != 1) {
        }
        if (CdRom_GetPendingReadCount() != 0) {
            continue;
        }
        func_80080D5C(2, &MEMCARD_VIDEO_FILE.pos, result);
        if (func_80081314(&MEMCARD_VIDEO_FILE.pos, 0x1E0)) {
            break;
        }
    }
    func_800870F0(D_800B0DBE);
    func_8010BD4C(MEMCARD_VIDEO_PTR4);
    for (;;) {
        retries = 2000;
        for (;;) {
            frame = MEMCARD_VIDEO_UPDATE_FRAME(&MEMCARD_VIDEO_DISPLAY);
            if (frame) {
                D_800B0DBC++;
                MEMCARD_VIDEO_DISPLAY.decodeIndex ^= 1;
                func_8010C89C(frame, MEMCARD_VIDEO_DISPLAY.decode[MEMCARD_VIDEO_DISPLAY.decodeIndex], MEMCARD_VIDEO_PTR4);
                func_8007C394(frame);
                status = 0;
                break;
            }
            if (--retries == 0) {
                status = -1;
                break;
            }
        }
        if ((s16)status != -1) {
            break;
        }
        MEMCARD_VIDEO_RESTART = MEMCARD_VIDEO_FILE.pos;
        for (;;) {
            while (func_8007F72C() != 1) {
            }
            if (CdRom_GetPendingReadCount() != 0) {
                continue;
            }
            func_80080D5C(2, &MEMCARD_VIDEO_RESTART, result);
            if (func_80081314(&MEMCARD_VIDEO_RESTART, 0x1E0)) {
                break;
            }
        }
    }
    MEMCARD_VIDEO_FINISHED = 0;
    D_800B0DBC = 1;
    D_800B0DBA++;
    return 1;
}
