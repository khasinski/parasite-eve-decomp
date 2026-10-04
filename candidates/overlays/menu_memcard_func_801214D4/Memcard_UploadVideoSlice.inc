#ifndef MEMCARD_UPLOAD_VIDEO_SLICE_NAME
#define MEMCARD_UPLOAD_VIDEO_SLICE_NAME Memcard_UploadVideoSlice
#define MEMCARD_VIDEO_UPLOAD D_801D1470
#define MEMCARD_VIDEO_REGIONS D_801D147A
#define MEMCARD_VIDEO_REGION D_801D148A
#define MEMCARD_VIDEO_RECT D_801D148C
#define MEMCARD_VIDEO_DONE D_801D1494
#define MEMCARD_VIDEO_EVENT_FLAG D_801D0DC0
#define MEMCARD_VIDEO_SET_DISPLAY func_801918F8
#endif

void MEMCARD_UPLOAD_VIDEO_SLICE_NAME(void) {
    VideoRect rectangle;
    s8 old;
    s32 next;
    s32 region;
    s16 x;

    if (D_800B0DBB && g_CdStreamReadyHalfword) {
        func_8007C564();
        g_CdStreamReadyHalfword = 0;
    }
    rectangle = MEMCARD_VIDEO_RECT;
    old = MEMCARD_VIDEO_UPLOAD.selector;
    next = MEMCARD_VIDEO_UPLOAD.selector ^ 1;
    MEMCARD_VIDEO_UPLOAD.selector = next;
    x = MEMCARD_VIDEO_RECT.x + MEMCARD_VIDEO_RECT.w;
    region = MEMCARD_VIDEO_REGION;
    MEMCARD_VIDEO_RECT.x = x;
    if (x < MEMCARD_VIDEO_REGIONS[region].x + MEMCARD_VIDEO_REGIONS[region].w) {
        func_8010C01C(MEMCARD_VIDEO_UPLOAD.buffers[next],
                      MEMCARD_VIDEO_RECT.w * MEMCARD_VIDEO_RECT.h / 2);
    } else {
        MEMCARD_VIDEO_DONE = 1;
        region ^= 1;
        MEMCARD_VIDEO_REGION = region;
        MEMCARD_VIDEO_RECT.x = MEMCARD_VIDEO_REGIONS[region].x;
        MEMCARD_VIDEO_RECT.y = MEMCARD_VIDEO_REGIONS[region].y;
        if (MEMCARD_VIDEO_EVENT_FLAG == 1) {
            D_800B0DBB ^= 1;
            if (D_800B0DBB) {
                MEMCARD_VIDEO_RECT.w = 24;
            } else {
                MEMCARD_VIDEO_RECT.w = 16;
            }
            MEMCARD_VIDEO_SET_DISPLAY(D_8009CDDC ^ 1, D_800B0DBB);
            MEMCARD_VIDEO_EVENT_FLAG = 2;
        }
    }
    func_8007506C(&rectangle, MEMCARD_VIDEO_UPLOAD.buffers[old]);
}

#undef MEMCARD_VIDEO_SET_DISPLAY
#undef MEMCARD_VIDEO_EVENT_FLAG
#undef MEMCARD_VIDEO_DONE
#undef MEMCARD_VIDEO_RECT
#undef MEMCARD_VIDEO_REGION
#undef MEMCARD_VIDEO_REGIONS
#undef MEMCARD_VIDEO_UPLOAD
#undef MEMCARD_UPLOAD_VIDEO_SLICE_NAME
