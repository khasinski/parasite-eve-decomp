#ifndef MEMCARD_SET_VIDEO_DISPLAY_NAME
#define MEMCARD_SET_VIDEO_DISPLAY_NAME Memcard_SetVideoDisplay
#define MEMCARD_VIDEO_DISPLAY_MODE D_801D0DBE
#endif

void MEMCARD_SET_VIDEO_DISPLAY_NAME(s8 index, s8 wide) {
    s16 y[2];

    if (index == 0) {
        y[0] = 240;
        y[1] = 0;
    } else {
        y[0] = 0;
        y[1] = 240;
    }
    if (wide) {
        MEMCARD_VIDEO_DISPLAY_MODE = 3;
        func_800749D8(&D_800BCE80[index], 0, y[0], 480, 240);
        D_800BCE80[index].isrgb24 = 1;
        D_800BCE80[index].disp.w = D_800BCE80[index].disp.w * 2 / 3;
        func_80074924(&D_800BCDC8[index], 0, y[1], 480, 240);
        D_800BCDC8[index].isbg = 1;
        D_800BCDC8[index].dtd = 1;
        D_800BCDC8[index].dfe = 0;
        D_800BCDC8[index].r0 = 0;
        D_800BCDC8[index].g0 = 0;
        D_800BCDC8[index].b0 = 0;
        D_800BCDC8[index].clip.w = D_800BCDC8[index].clip.w * 2 / 3;
    } else {
        MEMCARD_VIDEO_DISPLAY_MODE = 2;
        func_800749D8(&D_800BCE80[index], 0, y[0], 320, 240);
        func_80074924(&D_800BCDC8[index], 0, y[1], 320, 240);
        D_800BCDC8[index].isbg = 1;
        D_800BCDC8[index].dtd = 1;
        D_800BCDC8[index].dfe = 0;
        D_800BCDC8[index].r0 = 0;
        D_800BCDC8[index].g0 = 0;
        D_800BCDC8[index].b0 = 0;
    }
}

#undef MEMCARD_VIDEO_DISPLAY_MODE
#undef MEMCARD_SET_VIDEO_DISPLAY_NAME
