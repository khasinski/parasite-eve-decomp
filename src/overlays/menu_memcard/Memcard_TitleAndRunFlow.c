/* CC1_FLAGS: -fno-cse-skip-blocks */
#include "menu_memcard_screen.h"

/* Fade a two-part logo in and out over 480 frames on the flipping screens. */
void func_80190660(void) {
    MemcardSprite sprites[2][2];
    MemcardDrawTPage tpages[2][2];
    RECT overlay;
    MemcardTim *tim;
    MemcardTimBlock *block;
    MemcardSprite *sprite;
    MemcardDrawTPage *tpage;
    s32 i;
    s32 page;
    s32 shade;

    tim = (MemcardTim *)&D_80193254[D_80193278];
    func_8007506C(&tim->clut.rect, tim->clut.data);
    block = &tim->clut;
    block = (MemcardTimBlock *)((u32 *)block + (block->size >> 2));
    func_8007506C(&block->rect, block->data);
    DrawSync(0);

    for (i = 0; i < 2; i++) {
        sprite = sprites[i & 1];
        tpage = tpages[i & 1];
        func_80077C84(tpage, 0, 0, 0x18);
        sprite->len = 4;
        sprite->code = 0x64;
        sprite->x0 = 0x20;
        sprite->y0 = 0x58;
        sprite->u0 = sprite->v0 = 0;
        sprite->clut = 0x7800;
        sprite->w = 0x100;
        sprite->h = 0x40;
        sprite++;
        func_80077C84(tpage + 1, 0, 0, 0x19);
        sprite->len = 4;
        sprite->code = 0x64;
        sprite->x0 = 0x11C;
        sprite->y0 = 0x50;
        sprite->u0 = sprite->v0 = 0;
        sprite->clut = 0x7800;
        sprite->w = 8;
        sprite->h = 0x50;
    }

    {
        MemcardScreenBuffer *first = D_801D11BC[0];
        MemcardScreenBuffer *second = D_801D11BC[1];

        second->disp.isrgb24 = 0;
        first->disp.isrgb24 = 0;
    }
    SetDispMask(1);

    for (i = 0; i < 480; i++) {
        page = D_801D11C8 == 0;
        D_801D11C8 = page;
        D_801D11C4 = D_801D11BC[page];
        sprite = sprites[i & 1];
        tpage = tpages[i & 1];
        if (i < 32) {
            shade = i * 4;
        } else if (i < 424) {
            if (i >= 392) {
                shade = (424 - i) * 4;
            } else {
                shade = 0x80;
            }
        } else {
            shade = 0;
        }
        sprite->r0 = sprite->g0 = sprite->b0 = shade;
        func_80075358(tpage);
        func_80075358(sprite);
        sprite++;
        sprite->r0 = sprite->g0 = sprite->b0 = shade;
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
        VSync(0);
        func_80074A44(1);
        PutDrawEnv(&D_801D11C4->draw);
        func_800755F0(&D_801D11C4->disp);
    }

    SetDispMask(0);
    {
        MemcardScreenBuffer *first = D_801D11BC[0];
        MemcardScreenBuffer *second = D_801D11BC[1];

        second->disp.isrgb24 = 1;
        first->disp.isrgb24 = 1;
    }
}
/* CC1_FLAGS: -fno-cse-skip-blocks */
#include "menu_memcard_menu.h"

/* Run the title/card-selection screen, then restore the game display.
 * Five pins and the header's two symbol views retain retail allocation. */
s32 func_801909B4(void) {
    u8 *firstMemory;
    s32 searchKind;
    s32 wantedKind;
    register u32 prefix asm("$3");
    register u32 packedColor asm("$2");
    RECT rect;
    RECT overlay;
    MemcardImageNode *oldFreeHead;
    MemcardImageNode *previous;
    MemcardImageNode *selection;
    MemcardImageNode *nextFree;
    MemcardImageNode *freeEnd;
    MemcardImageNode *freeNode;
    MemcardImageNode *node;
    s32 selectionY;
    s32 screenYRestore;
    s32 titleMode;
    s32 elapsed;
    s32 page;
    s32 initialPage;
    s32 waitFrames;
    s32 result;
    s32 groupsRemaining;
    u32 rgb0;
    u32 rgb1;
    u32 rgb2;
    MemcardImage *image;
    register u8 *secondMemory asm("$5");
    u32 *source;
    u32 *destination;

    /* Save both display pages before borrowing their backing memory. */
    D_801D1550 = D_800BCE80;
    D_801D1564 = D_800BCE94;
    titleMode = D_800B0DCD & 1;
    D_801D1498 = D_800BCDC8;
    D_801D14F4 = D_800BCE24;
    firstMemory = D_80011610;
    secondMemory = firstMemory + 0x1C080;
    D_800B0E50 = firstMemory + 0x4080;
    D_801D11BC[0] = (MemcardScreenBuffer *) firstMemory;
    secondScreen.value = (MemcardScreenBuffer *) secondMemory;
    D_800B0E54 = secondMemory + 0x4080;
    D_800B0E38 = firstMemory + 0x80;
    D_800B0E3C = secondMemory + 0x80;
    func_8005E57C(1);
    result = -1;
    do {
        func_8005C1EC(1);
        func_80042538();
        SetDispMask(0);
        rect.x = 0x140;
        rect.w = 0xA0;
        rect.y = 0;
        rect.h = 0x100;
        func_8007512C(&rect, 0x2C0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        func_80074924(&firstDrawScreen->draw, 0, 0, 0x140, 0xF0);
        func_80074924(&D_801D11C0->draw, 0, 0xF0, 0x140, 0xF0);
        func_800749D8(&D_801D11BC[0]->disp, 0, 0xF0, 0x140, 0xF0);
        func_800749D8(&secondScreen.value->disp, 0, 0, 0x140, 0xF0);
        D_801D11BC[0]->disp.screen.y = secondScreen.value->disp.screen.y = 0;
        D_801D11BC[0]->disp.screen.h = secondScreen.value->disp.screen.h = 0xF0;
        D_801D11BC[0]->disp.isrgb24 = secondScreen.value->disp.isrgb24 = 1;
        D_801D11BC[0]->dirty.w = secondScreen.value->dirty.w = 0;
        D_801D11BC[0]->overlay.w = secondScreen.value->overlay.w = 0;
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x1E0;
        rect.h = 0x1E0;
        func_80074F44(&rect, 0, 0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        if (D_8009D1BC == 0) {
            D_8009D1BC = 1;
            func_80190660();
        }
        if (titleMode != 0) {
            if (func_80192CE8(1) != 0) {
                waitFrames = 0;
                do {
                    waitFrames += 1;
                    func_800425DC();
                    VSync(0);
                } while (waitFrames < 0x3C);
            }
            D_801D11BC[0]->draw = D_800BCDC8;
            secondScreen.value->draw = D_800BCE24;
            D_801D11BC[0]->disp = D_800BCE80;
            secondScreen.value->disp = D_800BCE94;
            D_801D11BC[0]->draw.isbg = 0;
            secondScreen.value->draw.isbg = 0;
            D_801D11BC[0]->dirty.w = secondScreen.value->dirty.w = 0;
            D_801D11C8 = D_8009CDDC;
            D_801D11BC[0]->overlay.w = secondScreen.value->overlay.w = 0;
            rect.x = 0;
            rect.y = 0;
            rect.w = 0x1E0;
            rect.h = 0x14;
            func_80074F44(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 0xE0;
            rect.w = 0x1E0;
            rect.h = 0x10;
            func_80074F44(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 0xF0;
            rect.w = 0x1E0;
            rect.h = 0x14;
            func_80074F44(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 0x1D0;
            rect.w = 0x1E0;
            rect.h = 0x10;
            func_80074F44(&rect, 0, 0, 0);
            initialPage = D_801D11C8 == 0;
            D_801D11C8 = initialPage;
            D_801D11C4 = D_801D11BC[initialPage];
            DrawSync(0);
            if (D_801D11C4->overlay.w > 0) {
                overlay = D_801D11C4->overlay;
                overlay.x = (s16) ((s32) (overlay.x * 3) >> 1);
                if (D_801D11C8 == 0) {
                    overlay.y += 0xF0;
                }
                overlay.w = (s16) ((s32) (overlay.w * 3) >> 1);
                func_8007506C(&overlay, D_801D11C4->overlayImage);
            }
            VSync(0);
            func_80074A44(1);
            PutDrawEnv(&D_801D11C4->draw);
            func_800755F0(&D_801D11C4->disp);
        }
        func_8018F2F4();
        VSync(0);
        SetDispMask(1);
        /* Rebuild the eight-node free list for this menu pass. */
        freeNode = D_801D11CC;
        freeEnd = &D_801D11CC[7];
        if ((u32) D_801D11CC < (u32) freeEnd) {
            do {
                nextFree = &freeNode[1];
                freeNode->next = nextFree;
                freeNode = nextFree;
            } while ((u32) freeNode < (u32) freeEnd);
        }
        freeNode->next = 0;
        D_801D136C = D_801D11CC;
        D_801D137C = 0;
        D_801D1378 = 0;
        D_801D1374 = 0;
        D_801D1370 = 0;
        func_8018FBC0(1)->field14 = D_8019319C;
        D_801D1380 = titleMode;
        if (titleMode < 0x3E8) {
            do {
                page = D_801D11C8 == 0;
                D_801D11C8 = page;
                D_801D11C4 = D_801D11BC[page];
                func_800425DC();
                func_8003EB04();
                func_80190064();
                func_8018F468();
                node = D_801D1370;
                previous = 0;
                if (node != 0) {
                    do {
                        if (node->field30 != 0) {
                            if (previous != 0) {
                                previous->next = node->next;
                            } else {
                                D_801D1370 = node->next;
                            }
                            if (D_801D1374 == node) {
                                D_801D1374 = previous;
                            }
                            oldFreeHead = D_801D136C;
                            D_801D136C = node;
                            node->next = oldFreeHead;
                            if (previous != 0) {
                                node = previous->next;
                            } else {
                                node = D_801D1370;
                            }
                        } else {
                            previous = node;
                            node = node->next;
                        }
                    } while (node != 0);
                }
                {
                    MemcardImageNode *pending = D_801D1378;
                    if (pending != 0) {
                        MemcardImageNode *tail = D_801D1374;
                        register MemcardImageNode *nextTail asm("$2");

                        if (tail != 0) {
                            nextTail = D_801D137C;
                            tail->next = pending;
                        } else {
                            nextTail = D_801D137C;
                            D_801D1370 = pending;
                        }
                        D_801D1374 = nextTail;
                        D_801D137C = 0;
                        D_801D1378 = 0;
                    }
                }
                DrawSync(0);
                if (D_801D11C4->overlay.w > 0) {
                    overlay = D_801D11C4->overlay;
                    overlay.x = (s16) ((s32) (overlay.x * 3) >> 1);
                    if (D_801D11C8 == 0) {
                        overlay.y += 0xF0;
                    }
                    overlay.w = (s16) ((s32) (overlay.w * 3) >> 1);
                    func_8007506C(&overlay, D_801D11C4->overlayImage);
                }
                VSync(2);
                func_80074A44(1);
                PutDrawEnv(&D_801D11C4->draw);
                func_800755F0(&D_801D11C4->disp);
                elapsed = D_801D1380 + titleMode;
                D_801D1380 = elapsed;
            } while (elapsed < 0x3E8);
        }
        SetDispMask(0);
        rect.x = 0x2C0;
        rect.w = 0xA0;
        rect.y = 0;
        rect.h = 0x100;
        func_8007512C(&rect, 0x140, 0);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x140;
        rect.h = 0x1E0;
        func_80074F44(&rect, 0, 0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        screenYRestore = 8;
        Draw_InitBuffers();
        func_8005E6F0();
        func_8005E788(2);
        if (D_801D1380 >= 0x3E9) {
            wantedKind = 7;
            selection = D_801D1370;
            while (selection != 0) {
                searchKind = selection->kind;
                if (searchKind == wantedKind) {
                    break;
                }
                selection = selection->next;
            }
            selectionY = selection->y;
            if (selectionY == 0xA0) {
                result = 1;
                func_80036E34();
                func_80036DF8();
            } else if ((selectionY >= 0xB5) || ((selectionY == 0xB4) && (func_80042770(0) == 0) && (func_80042770(1) == 0))) {
                result = 3;
            } else {
                func_8005E6E4((u8 *)D_801D11C0 + 0x1C080);
                image = (MemcardImage *)&D_80193254[*D_80193258];
                source = (u32 *)(image + 1);
                groupsRemaining = (image->width * image->height) / 6;
                destination = (u32 *)((u8 *)D_801D11C0 + 0x1C080);
                /* Pack four RGB24 pixels into two dimmed RGB555 words.
                 * The inclusive last group is intentional: retail uses >= 0. */
                if (groupsRemaining >= 0) {
                    do {
                        rgb0 = *source++;
                        rgb1 = *source++;
                        rgb2 = *source++;
                        groupsRemaining -= 1;
                        prefix = ((rgb0 >> 3) & 0x1F);
                        prefix |= ((rgb0 >> 6) & 0x3E0);
                        prefix |= ((rgb0 >> 9) & 0x7C00);
                        prefix |= ((rgb0 >> 0xB) & 0x1F0000);
                        prefix |= ((rgb1 << 0x12) & 0x03E00000);
                        packedColor = ((prefix | ((rgb1 << 0xF) & 0x7C000000)) >> 2) & 0x1CE71CE7;
                        *destination++ = packedColor;
                        prefix = ((rgb1 >> 0x13) & 0x1F);
                        prefix |= ((rgb1 >> 0x16) & 0x3E0);
                        prefix |= ((rgb2 << 7) & 0x7C00);
                        prefix |= ((rgb2 << 5) & 0x1F0000);
                        prefix |= ((rgb2 * 4) & 0x03E00000);
                        packedColor = ((prefix | ((rgb2 >> 1) & 0x7C000000)) >> 2) & 0x1CE71CE7;
                        *destination++ = packedColor;
                    } while (groupsRemaining >= 0);
                }
                SetDispMask(1);
                func_8004D084(0);
                func_8003FFAC(selectionY == 0x8C);
                do {
                    func_8003EB04();
                    result = func_8005C498(0);
                } while (result == 0);
                if ((result == 2) && (selectionY == 0x8C)) {
                    func_80036E34();
                }
                SetDispMask(0);
                func_8005E6E4(0);
                rect.x = 0;
                rect.y = 0;
                rect.w = 0x140;
                rect.h = 0x1E0;
                func_80074F44(&rect, 0, 0, 0);
                DrawSync(0);
                screenYRestore = D_800BCE8A;
            }
        }
    } while (result < 0);
    D_800BCDC8 = D_801D1498;
    D_800BCE24 = D_801D14F4;
    D_800BCE80 = D_801D1550;
    D_800BCE94 = D_801D1564;
    {
        register s32 screenY asm("$2") = screenYRestore;
        D_800BCE9E = screenY;
        D_800BCE8A = screenY;
    }
    func_8005C1EC(0);
    func_8005E57C(0);
    return result;
}
