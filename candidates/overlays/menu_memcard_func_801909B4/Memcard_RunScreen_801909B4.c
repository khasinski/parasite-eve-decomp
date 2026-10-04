#include "menu_memcard_image.h"
#include "menu_memcard_screen.h"

static inline MemcardImageNode *Memcard_FindImageNode(s32 kind) {
    MemcardImageNode *node;

    for (node = D_801D1370; node != 0; node = node->next) {
        if (node->kind == kind) {
            break;
        }
    }
    return node;
}

static inline void Memcard_PresentScreen(s32 vsyncMode) {
    RECT overlay;

    DrawSync(0);
    if (D_801D11C4->overlay.w > 0) {
        overlay = D_801D11C4->overlay;
        overlay.x = overlay.x * 3 >> 1;
        if (D_801D11C8 == 0) {
            overlay.y += 240;
        }
        overlay.w = overlay.w * 3 >> 1;
        func_8007506C(&overlay, D_801D11C4->overlayImage);
    }
    VSync(vsyncMode);
    func_80074A44(1);
    PutDrawEnv(&D_801D11C4->draw);
    func_800755F0(&D_801D11C4->disp);
}

/* Runs the memory card screen until the player leaves it; returns the choice. */
s32 func_801909B4(void) {
    RECT rect;
    MemcardScreenBuffer *base;
    MemcardScreenBuffer *second;
    MemcardImageNode *node;
    MemcardImageNode *prev;
    MemcardImage *image;
    u32 *source;
    u32 *destination;
    s32 result;
    s32 hasVideo;
    s32 cursor;
    s32 screenY;
    s32 count;
    s32 page;
    s32 i;

    D_801D1550 = D_800BCE80[0];
    D_801D1564 = D_800BCE80[1];
    hasVideo = D_800B0DCD & 1;
    D_801D1498 = D_800BCDC8[0];
    D_801D14F4 = D_800BCDC8[1];
    result = -1;
    base = (MemcardScreenBuffer *)D_80011610;
    second = base + 1;
    D_800B0E50 = base->work4080;
    D_801D11BC[0] = base;
    D_801D11BC[1] = second;
    D_800B0E54 = second->work4080;
    D_800B0E38 = base->work80;
    D_800B0E3C = second->work80;
    func_8005E57C(1);
    do {
        func_8005C1EC(1);
        func_80042538();
        SetDispMask(0);
        rect.x = 320;
        rect.y = 0;
        rect.w = 160;
        rect.h = 256;
        func_8007512C(&rect, 704, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        func_80074924(&D_801D11BC[0]->draw, 0, 0, 320, 240);
        func_80074924(&D_801D11BC[1]->draw, 0, 240, 320, 240);
        func_800749D8(&D_801D11BC[0]->disp, 0, 240, 320, 240);
        func_800749D8(&D_801D11BC[1]->disp, 0, 0, 320, 240);
        {
            MemcardScreenBuffer *first = D_801D11BC[0];
            MemcardScreenBuffer *second = D_801D11BC[1];

            second->disp.screen.y = 0;
            first->disp.screen.y = 0;
        }
        {
            MemcardScreenBuffer *first = D_801D11BC[0];
            MemcardScreenBuffer *second = D_801D11BC[1];

            second->disp.screen.h = 240;
            first->disp.screen.h = 240;
        }
        {
            MemcardScreenBuffer *first = D_801D11BC[0];
            MemcardScreenBuffer *second = D_801D11BC[1];

            second->disp.isrgb24 = 1;
            first->disp.isrgb24 = 1;
        }
        {
            MemcardScreenBuffer *first = D_801D11BC[0];
            MemcardScreenBuffer *second = D_801D11BC[1];

            second->dirty.w = 0;
            first->dirty.w = 0;
        }
        {
            MemcardScreenBuffer *first = D_801D11BC[0];
            MemcardScreenBuffer *second = D_801D11BC[1];

            second->overlay.w = 0;
            first->overlay.w = 0;
        }
        rect.x = 0;
        rect.y = 0;
        rect.w = 480;
        rect.h = 480;
        func_80074F44(&rect, 0, 0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        if (D_8009D1BC == 0) {
            D_8009D1BC = 1;
            func_80190660();
        }
        if (hasVideo) {
            if (Memcard_PlayVideo(1) != 0) {
                for (i = 0; i < 60; i++) {
                    func_800425DC();
                    VSync(0);
                }
            }
            D_801D11BC[0]->draw = D_800BCDC8[0];
            D_801D11BC[1]->draw = D_800BCDC8[1];
            D_801D11BC[0]->disp = D_800BCE80[0];
            D_801D11BC[1]->disp = D_800BCE80[1];
            D_801D11BC[0]->draw.isbg = 0;
            D_801D11BC[1]->draw.isbg = 0;
            {
                MemcardScreenBuffer *first = D_801D11BC[0];
                MemcardScreenBuffer *second = D_801D11BC[1];

                second->dirty.w = 0;
                first->dirty.w = 0;
            }
            D_801D11C8 = D_8009CDDC;
            {
                MemcardScreenBuffer *first = D_801D11BC[0];
                MemcardScreenBuffer *second = D_801D11BC[1];

                second->overlay.w = 0;
                first->overlay.w = 0;
            }
            rect.x = 0;
            rect.y = 0;
            rect.w = 480;
            rect.h = 20;
            func_80074F44(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 224;
            rect.w = 480;
            rect.h = 16;
            func_80074F44(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 240;
            rect.w = 480;
            rect.h = 20;
            func_80074F44(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 464;
            rect.w = 480;
            rect.h = 16;
            func_80074F44(&rect, 0, 0, 0);
            page = D_801D11C8 == 0;
            D_801D11C8 = page;
            D_801D11C4 = D_801D11BC[page];
            Memcard_PresentScreen(0);
        }
        func_80127848();
        VSync(0);
        SetDispMask(1);
        for (node = D_801D11CC; node < &D_801D11CC[7]; node++) {
            node->next = node + 1;
        }
        node->next = 0;
        D_801D136C = D_801D11CC;
        D_801D137C = 0;
        D_801D1378 = 0;
        D_801D1374 = 0;
        D_801D1370 = 0;
        Memcard_CreateImageNode(1)->field14 = D_8019319C;
        D_801D1380 = hasVideo;
        if (hasVideo < 1000) {
            do {
                page = D_801D11C8 == 0;
                D_801D11C8 = page;
                D_801D11C4 = D_801D11BC[page];
                func_800425DC();
                func_8003EB04();
                func_80190064();
                func_8018F468();
                prev = 0;
                node = D_801D1370;
                while (node != 0) {
                    if (node->field30 != 0) {
                        if (prev != 0) {
                            prev->next = node->next;
                        } else {
                            D_801D1370 = node->next;
                        }
                        if (D_801D1374 == node) {
                            D_801D1374 = prev;
                        }
                        node->next = D_801D136C;
                        D_801D136C = node;
                        if (prev != 0) {
                            node = prev->next;
                        } else {
                            node = D_801D1370;
                        }
                    } else {
                        prev = node;
                        node = node->next;
                    }
                }
                if (D_801D1378 != 0) {
                    if (D_801D1374 != 0) {
                        D_801D1374->next = D_801D1378;
                    } else {
                        D_801D1370 = D_801D1378;
                    }
                    D_801D1374 = D_801D137C;
                    D_801D137C = 0;
                    D_801D1378 = 0;
                }
                Memcard_PresentScreen(2);
                D_801D1380 += hasVideo;
            } while (D_801D1380 < 1000);
        }
        SetDispMask(0);
        rect.x = 704;
        rect.y = 0;
        rect.w = 160;
        rect.h = 256;
        func_8007512C(&rect, 320, 0);
        rect.x = 0;
        rect.y = 0;
        rect.w = 320;
        rect.h = 480;
        func_80074F44(&rect, 0, 0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        Draw_InitBuffers();
        screenY = 8;
        func_8005E6F0();
        func_8005E788(2);
        if (D_801D1380 > 1000) {
            cursor = Memcard_FindImageNode(7)->y;
            if (cursor == 160) {
                result = 1;
                func_80036E34();
                func_80036DF8();
            } else if (cursor >= 181 ||
                       (cursor == 180 && func_80042770(0) == 0 && func_80042770(1) == 0)) {
                result = 3;
            } else {
                func_8005E6E4(D_801D11BC[1] + 1);
                image = (MemcardImage *)&D_80193254[D_80193258[0]];
                count = image->width * image->height / 6;
                source = (u32 *)(image + 1);
                destination = (u32 *)(D_801D11BC[1] + 1);
                for (; count >= 0; count--) {
                    u32 a = *source++;
                    u32 b = *source++;
                    u32 c = *source++;

                    *destination++ = ((((a >> 3) & 0x1F) | ((a >> 6) & 0x3E0) | ((a >> 9) & 0x7C00) |
                                       ((a >> 11) & 0x1F0000) | ((b << 18) & 0x3E00000) |
                                       ((b << 15) & 0x7C000000)) >> 2) & 0x1CE71CE7;
                    *destination++ = ((((b >> 19) & 0x1F) | ((b >> 22) & 0x3E0) | ((c << 7) & 0x7C00) |
                                       ((c << 5) & 0x1F0000) | ((c << 2) & 0x3E00000) |
                                       ((c >> 1) & 0x7C000000)) >> 2) & 0x1CE71CE7;
                }
                SetDispMask(1);
                func_8004D084(0);
                func_8003FFAC(cursor == 140);
                do {
                    func_8003EB04();
                    result = func_8005C498(0);
                } while (result == 0);
                if (result == 2 && cursor == 140) {
                    func_80036E34();
                }
                SetDispMask(0);
                func_8005E6E4(0);
                rect.x = 0;
                rect.y = 0;
                rect.w = 320;
                rect.h = 480;
                func_80074F44(&rect, 0, 0, 0);
                DrawSync(0);
                screenY = D_800BCE80[0].screen.y;
            }
        }
    } while (result < 0);
    D_800BCDC8[0] = D_801D1498;
    D_800BCDC8[1] = D_801D14F4;
    D_800BCE80[0] = D_801D1550;
    D_800BCE80[1] = D_801D1564;
    D_800BCE80[1].screen.y = screenY;
    D_800BCE80[0].screen.y = screenY;
    func_8005C1EC(0);
    func_8005E57C(0);
    return result;
}
