#ifndef MEMCARD_INIT_VIDEO_BUFFERS_NAME
#define MEMCARD_INIT_VIDEO_BUFFERS_NAME Memcard_InitVideoBuffers
#define MEMCARD_VIDEO_PTR0 D_801D0DE8
#define MEMCARD_VIDEO_PTR1 D_801D0DEC
#define MEMCARD_VIDEO_PTR2 D_801D0DF0
#define MEMCARD_VIDEO_PTR3 D_801D0DF4
#define MEMCARD_VIDEO_PTR4 D_801D0DF8
#define MEMCARD_VIDEO_PTR5 D_801D0DFC
#define MEMCARD_VIDEO_LAST_TIME D_801D11B0
#define MEMCARD_VIDEO_SAVED_DISPLAY D_801D1384
#define MEMCARD_VIDEO_SAVED_DRAW D_801D13AC
#endif

s32 MEMCARD_INIT_VIDEO_BUFFERS_NAME(s8 count, u8 **buffers) {
    s8 i;
    VideoRect rectangle;
    u8 *base;
    s32 offset;

    if ((u8)(count - 1) >= 2) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        if (!buffers[i]) {
            return 0;
        }
    }
    if (D_800B0DBA) {
        return 0;
    }
    switch (count) {
        case 1:
            base = buffers[0];
            MEMCARD_VIDEO_PTR0 = base;
            offset = 0xFA00;
            MEMCARD_VIDEO_PTR1 = base + offset;
            offset = 0x1F400;
            MEMCARD_VIDEO_PTR5 = base + offset;
            offset = 0x3F400;
            MEMCARD_VIDEO_PTR4 = base + offset;
            offset = 0x50400;
            MEMCARD_VIDEO_PTR2 = base + offset;
            offset = 0x53100;
            MEMCARD_VIDEO_PTR3 = base + offset;
            break;
        case 2:
            base = buffers[0];
            MEMCARD_VIDEO_PTR0 = base;
            offset = 0xFA00;
            MEMCARD_VIDEO_PTR1 = base + offset;
            offset = 0x1F400;
            MEMCARD_VIDEO_PTR5 = base + offset;
            buffers++;
            base = *buffers;
            offset = 0x11000;
            MEMCARD_VIDEO_PTR2 = base + offset;
            MEMCARD_VIDEO_PTR4 = base;
            offset = 0x13D00;
            MEMCARD_VIDEO_PTR3 = base + offset;
            break;
    }
    D_800B0DBA = 1;
    D_800B0DBE = 152;
    D_800B0DBC = 0;
    MEMCARD_VIDEO_LAST_TIME = -1;
    for (i = 0; i < 2; i++) {
        MEMCARD_VIDEO_SAVED_DISPLAY[i] = D_800BCE80[i];
        MEMCARD_VIDEO_SAVED_DRAW[i] = D_800BCDC8[i];
    }
    rectangle.x = 320;
    rectangle.y = 0;
    rectangle.w = 192;
    rectangle.h = 256;
    func_8007512C(&rectangle, 512, 0);
    if (!(D_800B0CD8 & 0x08000000)) {
        rectangle.x = 0;
        rectangle.y = 448;
        rectangle.w = 320;
        rectangle.h = 64;
        func_8007512C(&rectangle, 512, 256);
    }
    return 1;
}

#undef MEMCARD_VIDEO_SAVED_DRAW
#undef MEMCARD_VIDEO_SAVED_DISPLAY
#undef MEMCARD_VIDEO_LAST_TIME
#undef MEMCARD_VIDEO_PTR5
#undef MEMCARD_VIDEO_PTR4
#undef MEMCARD_VIDEO_PTR3
#undef MEMCARD_VIDEO_PTR2
#undef MEMCARD_VIDEO_PTR1
#undef MEMCARD_VIDEO_PTR0
#undef MEMCARD_INIT_VIDEO_BUFFERS_NAME
