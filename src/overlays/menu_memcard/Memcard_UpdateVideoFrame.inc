#ifndef MEMCARD_UPDATE_VIDEO_FRAME_NAME
#define MEMCARD_UPDATE_VIDEO_FRAME_NAME Memcard_UpdateVideoFrame
#define MEMCARD_VIDEO_ENTRY D_801D11AC
#define MEMCARD_VIDEO_LAST_TIME D_801D11B0
#define MEMCARD_VIDEO_FINISHED D_801D0DBD
#define MEMCARD_VIDEO_SIZE D_801D0DE0
#endif

void *MEMCARD_UPDATE_VIDEO_FRAME_NAME(VideoDisplay *display) {
    struct {
        void *data;
        VideoFrame *frame;
        VideoRect clear;
    } local;
    s32 retries;
    s32 end;
    s32 remaining;
    s32 status;
    void *data;
    /* Retail never reloads $a0 after the event-sync call below: the frame
       pointer argument lives in $a0 across it, which only a $4 variable
       reproduces. */
    register void **dataPtr asm("$4");

    retries = 2000;
    status = func_8007F72C();
    dataPtr = &local.data;
    if (status == 1) {
        status = func_8007F7A8();
        dataPtr = &local.data;
        if (status != D_800B0DD4) {
            func_800719E4(1);
        }
    }
    while (func_8007C484(dataPtr, &local.frame) != 0) {
        dataPtr = &local.data;
        if (--retries == 0) {
            return 0;
        }
    }
    end = MEMCARD_VIDEO_ENTRY->end;
    if (local.frame->time >= (u32)(end - 16)) {
        s32 position = local.frame->time + 16;

        remaining = position - end;
        remaining = 14 - remaining;
        if (remaining < 0) {
            remaining = 0;
        }
        func_800870F0(D_800B0DBE * remaining / 14);
    }
    if (local.frame->time < (u32)MEMCARD_VIDEO_LAST_TIME || local.frame->time >= (u32)MEMCARD_VIDEO_ENTRY->end) {
        MEMCARD_VIDEO_FINISHED = 1;
    }
    MEMCARD_VIDEO_LAST_TIME = local.frame->time;
    if (MEMCARD_VIDEO_SIZE[0] != local.frame->width || MEMCARD_VIDEO_SIZE[1] != local.frame->height) {
        if (D_800B0DBB) {
            local.clear.x = 0;
            local.clear.y = 0;
            local.clear.w = 480;
            local.clear.h = 480;
        } else {
            local.clear.x = 0;
            local.clear.y = 0;
            local.clear.w = 320;
            local.clear.h = 480;
        }
        func_80074F44(&local.clear, 0, 0, 0);
        MEMCARD_VIDEO_SIZE[0] = local.frame->width;
        MEMCARD_VIDEO_SIZE[1] = local.frame->height;
    }
    if (D_800B0DBB) {
        display->width1a = display->width22 = MEMCARD_VIDEO_SIZE[0] * 3 / 2;
    } else {
        display->width1a = display->width22 = MEMCARD_VIDEO_SIZE[0];
    }
    data = local.data;
    display->height1c = display->height24 = MEMCARD_VIDEO_SIZE[1];
    display->height2e = MEMCARD_VIDEO_SIZE[1];
    return data;
}

#undef MEMCARD_VIDEO_SIZE
#undef MEMCARD_VIDEO_FINISHED
#undef MEMCARD_VIDEO_LAST_TIME
#undef MEMCARD_VIDEO_ENTRY
#undef MEMCARD_UPDATE_VIDEO_FRAME_NAME
