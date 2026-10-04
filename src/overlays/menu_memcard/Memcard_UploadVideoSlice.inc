#ifndef MEMCARD_UPLOAD_VIDEO_SLICE_NAME
#define MEMCARD_UPLOAD_VIDEO_SLICE_NAME Memcard_UploadVideoSlice
#define MEMCARD_VIDEO_DISPLAY D_801D1464
#define MEMCARD_VIDEO_FLIP D_801D0DC0
#define MEMCARD_VIDEO_SET_DISPLAY func_801918F8
#endif

/* Slice callback of the PSY-Q movie player: upload the decoded slice to
 * VRAM, flip the slice buffers and start the next slice. When the frame is
 * complete, switch to the other display region and apply a pending colour
 * depth change (24-bit slices are 24 pixels wide, 16-bit ones 16). */
void MEMCARD_UPLOAD_VIDEO_SLICE_NAME(void) {
    VideoRect rectangle;
    s8 old;

    if (D_800B0DBB && g_CdStreamReadyHalfword) {
        func_8007C564();
        g_CdStreamReadyHalfword = 0;
    }
    old = MEMCARD_VIDEO_DISPLAY.selector;
    rectangle = MEMCARD_VIDEO_DISPLAY.rect;
    MEMCARD_VIDEO_DISPLAY.selector ^= 1;
    MEMCARD_VIDEO_DISPLAY.rect.x += MEMCARD_VIDEO_DISPLAY.rect.w;
    if (MEMCARD_VIDEO_DISPLAY.rect.x
        < MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].x
              + MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].w) {
        func_8010C01C(MEMCARD_VIDEO_DISPLAY.buffers[MEMCARD_VIDEO_DISPLAY.selector],
                      MEMCARD_VIDEO_DISPLAY.rect.w * MEMCARD_VIDEO_DISPLAY.rect.h / 2);
    } else {
        MEMCARD_VIDEO_DISPLAY.done = 1;
        MEMCARD_VIDEO_DISPLAY.region ^= 1;
        MEMCARD_VIDEO_DISPLAY.rect.x = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].x;
        MEMCARD_VIDEO_DISPLAY.rect.y = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].y;
        if (MEMCARD_VIDEO_FLIP == 1) {
            s8 *wide = &D_800B0DBB;

            *wide ^= 1;
            if (*wide) {
                MEMCARD_VIDEO_DISPLAY.rect.w = 24;
            } else {
                MEMCARD_VIDEO_DISPLAY.rect.w = 16;
            }
            MEMCARD_VIDEO_SET_DISPLAY(D_8009CDDC ^ 1, D_800B0DBB);
            MEMCARD_VIDEO_FLIP = 2;
        }
    }
    func_8007506C(&rectangle, MEMCARD_VIDEO_DISPLAY.buffers[old]);
}
