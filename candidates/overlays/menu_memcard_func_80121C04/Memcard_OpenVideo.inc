/* Open catalog video `index`: point the player at its entry, find the file
 * on disc, start streaming it and wait for the first decoded frame,
 * re-seeking to the saved position whenever 2000 polls pass without one. */
s32 MEMCARD_OPEN_VIDEO_NAME(s16 index)
{
    VideoRect origin;
    char name[32];
    u8 result[8];
    u16 number;
    s32 found;
    void *frame;
    s16 timeout;
    s16 status;

    number = index;
    if (number >= 47) {
        return 0;
    }
    D_800B0DBF = index;
    MEMCARD_VIDEO_ENTRY = &MEMCARD_VIDEO_CATALOG[index];
    D_800B0DBB = MEMCARD_VIDEO_ENTRY->enabled;
    MEMCARD_SET_VIDEO_DISPLAY(0, D_800B0DBB);
    MEMCARD_SET_VIDEO_DISPLAY(1, D_800B0DBB);
    name[0] = 0;
    if (number < 21) {
        func_800719F4(name, MEMCARD_VIDEO_DIR_A);
    } else {
        func_800719F4(name, MEMCARD_VIDEO_DIR_B);
    }
    func_800719F4(name, MEMCARD_VIDEO_ENTRY->path);
    for (;;) {
        found = 0;
        if (func_8007F72C() == 1 && CdRom_GetPendingReadCount() == 0) {
            found = func_80081414(&MEMCARD_VIDEO_LOC, name);
        }
        if (found == 0) {
            continue;
        }
        if (found != -1) {
            break;
        }
    }
    MEMCARD_VIDEO_SAVED_LOC = MEMCARD_VIDEO_LOC;
    MEMCARD_VIDEO_DISPLAY.decode[0] = MEMCARD_VIDEO_PTR0;
    MEMCARD_VIDEO_DISPLAY.decodeIndex = 0;
    MEMCARD_VIDEO_DISPLAY.buffers[0] = MEMCARD_VIDEO_PTR2;
    MEMCARD_VIDEO_DISPLAY.buffers[1] = MEMCARD_VIDEO_PTR3;
    MEMCARD_VIDEO_DISPLAY.selector = 0;
    origin.x = MEMCARD_VIDEO_ENTRY->x;
    origin.y = MEMCARD_VIDEO_ENTRY->y;
    MEMCARD_VIDEO_DISPLAY.regions[0].x = origin.x;
    MEMCARD_VIDEO_DISPLAY.regions[0].y = origin.y + 0xF0;
    MEMCARD_VIDEO_DISPLAY.regions[1].x = origin.x;
    MEMCARD_VIDEO_DISPLAY.regions[1].y = origin.y;
    MEMCARD_VIDEO_DISPLAY.decode[1] = MEMCARD_VIDEO_PTR1;
    MEMCARD_VIDEO_DISPLAY.region = D_8009CDDC;
    MEMCARD_VIDEO_DISPLAY.rect.x = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].x;
    MEMCARD_VIDEO_DISPLAY.rect.y = MEMCARD_VIDEO_DISPLAY.regions[MEMCARD_VIDEO_DISPLAY.region].y;
    MEMCARD_VIDEO_DISPLAY.rect.w = D_800B0DBB ? 0x18 : 0x10;
    MEMCARD_VIDEO_DISPLAY.done = 0;
    func_8010BE3C(0);
    func_8010C0D8(MEMCARD_VIDEO_UPLOAD);
    func_8007A214(MEMCARD_VIDEO_PTR5, 0x40);
    func_8007C304(1, MEMCARD_VIDEO_ENTRY->channel, -1, 0, 0);
    do {
        while (func_8007F72C() != 1) {
        }
    } while (CdRom_GetPendingReadCount() != 0
             || (func_80080D5C(2, &MEMCARD_VIDEO_LOC, result),
                 func_80081314(&MEMCARD_VIDEO_LOC, 0x1E0) == 0));
    func_800870F0(D_800B0DBE);
    func_8010BD4C(MEMCARD_VIDEO_PTR4);
    timeout = 2000;
    for (;;) {
        frame = MEMCARD_UPDATE_VIDEO_FRAME(&MEMCARD_VIDEO_DISPLAY);
        if (frame == 0) {
            if (--timeout != 0) {
                continue;
            }
            status = -1;
        } else {
            D_800B0DBC++;
            MEMCARD_VIDEO_DISPLAY.decodeIndex ^= 1;
            func_8010C89C(frame, MEMCARD_VIDEO_DISPLAY.decode[MEMCARD_VIDEO_DISPLAY.decodeIndex]);
            func_8007C394(frame);
            status = 0;
        }
        if (status != -1) {
            break;
        }
        MEMCARD_VIDEO_SAVED_LOC = MEMCARD_VIDEO_LOC;
        do {
            while (func_8007F72C() != 1) {
            }
        } while (CdRom_GetPendingReadCount() != 0
                 || (func_80080D5C(2, &MEMCARD_VIDEO_SAVED_LOC, result),
                     func_80081314(&MEMCARD_VIDEO_SAVED_LOC, 0x1E0) == 0));
        timeout = 2000;
    }
    MEMCARD_VIDEO_FINISHED = 0;
    D_800B0DBC = 1;
    D_800B0DBA++;
    return 1;
}
