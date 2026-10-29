/* CC1_FLAGS: -fno-cse-skip-blocks */

#include "menu_memcard_image.h"
#include "menu_memcard_menu.h"
#include "menu_memcard_screen.h"
extern RenderBufferPrefix D_800B08F8;

static inline MemcardImageNode *MenuCard_FindImageNode(s32 kind) {
    MemcardImageNode *node;

    for (node = (g_MemcardActiveNodeHead); node != 0; node = node->next) {
        if (node->kind == kind) {
            break;
        }
    }
    return node;
}

/* Scroll and confirm handling for the memory card list screen. */
void MenuCard_HandleListInput(void) {
    s32 pad;
    s32 y;
    MemcardImageNode *node;

    pad = Draw_RemapStatusFlags();
    if ((pad & 0x800) && !(g_MemcardPrevPad & 0x800)) {
        node = MenuCard_FindImageNode(2);
        if (node != 0 && node->intensity > 0x80) {
            node = MenuCard_FindImageNode(1);
            if (node->stepY == 0) {
                node->stepY = 1;
                node->parameter = 8;
                node->field14 = g_MenuCardAttachFadePayloadEntry;
                g_MemcardScreenTimer = 0;
                Menu_PlayConfirmSound();
            }
        }
    }
    node = MenuCard_FindImageNode(5);
    if (node != 0 && node->draw == 0) {
        if (MemCard_IsPortPresent(0) != 0 || MemCard_IsPortPresent(1) != 0) {
            if (node->y < 200) {
                node->y++;
                MenuCard_FindImageNode(4)->targetY = 16;
                g_MemcardScreenTimer = 0;
            }
            if (node->y == 200 && MemCard_HasAnyPresentFile() != 0) {
                node = MenuCard_FindImageNode(6);
                if (node->targetY == 0) {
                    node->draw = g_MenuCardUpdateFadeDisplayEntry;
                    node->update = MenuCard_BlendRotatedImage;
                    node->targetY = 20;
                    node->stepY = 1;
                }
            }
        } else if (node->y >= 181) {
            node->y--;
            if (MenuCard_FindImageNode(7)->y > node->y) {
                MenuCard_FindImageNode(7)->y = node->y;
            }
            MenuCard_FindImageNode(4)->targetY = -16;
            g_MemcardScreenTimer = 0;
        }
        if (MemCard_HasAnyPresentFile() == 0) {
            node = MenuCard_FindImageNode(6);
            if (node->targetY == 0x54) {
                node->draw = g_MenuCardDecreaseFadeEntry;
            }
            if (node->intensity != 0) {
                node->stepY = -1;
                if (MenuCard_FindImageNode(7)->y == 140) {
                    MenuCard_FindImageNode(7)->y = 160;
                }
            }
        }
        node = MenuCard_FindImageNode(7);
        if (pad & 0x20) {
            if (node->y == 160 || node->y == 180 || node->y == 200 || node->y == 140) {
                g_MemcardScreenTimer = 1001;
                Menu_PlayConfirmSound();
            }
        }
        if ((pad & 0x1000) && !(g_MemcardPrevPad & 0x1000)) {
            y = node->y;
            if (MenuCard_FindImageNode(6)->intensity == 0x100 ? y >= 141 : y >= 161) {
                g_MemcardScreenTimer = 0;
                node->y -= 20;
                Menu_PlayMoveSound();
            }
        }
        if ((pad & 0x4000) && !(g_MemcardPrevPad & 0x4000)) {
            y = node->y;
            if (y <= MenuCard_FindImageNode(5)->y - 20) {
                g_MemcardScreenTimer = 0;
                node->y += 20;
                Menu_PlayMoveSound();
            }
        }
    }
    g_MemcardPrevPad = pad;
}

/* Run the title/card-selection screen, then restore the game display.
 * Three pins and the header's two symbol views retain retail allocation. */
s32 MenuCard_RunListScreen(void) {
    u8 *firstMemory;
    s32 searchKind;
    s32 wantedKind;
    register u32 prefix asm("$3");
    register u32 packedColor asm("$2");
    RECT rect;
    RECT overlay;
    /* One link temporary for the free-list build, unlink and append steps;
     * sharing it gives the retail register allocation. */
    MemcardImageNode *link;
    MemcardImageNode *previous;
    MemcardImageNode *selection;
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
    g_MemcardListSavedDispEnv[0] = g_RenderDispEnvArray[0];
    g_MemcardListSavedDispEnv[1] = g_RenderDispEnvArray[1];
    titleMode = g_PeImageMountFlags & 1;
    g_MemcardListSavedDrawEnv[0] = g_RenderDrawEnvArray[0];
    g_MemcardListSavedDrawEnv[1] = g_RenderDrawEnvArray[1];
    firstMemory = g_CdLoadAreaBase;
    secondMemory = firstMemory + 0x1C080;
    g_RenderFrontBufferBase[0] = firstMemory + 0x4080;
    g_MemcardScreenBuffers[0] = (MemcardScreenBuffer *) firstMemory;
    g_MemcardSecondScreen.value = (MemcardScreenBuffer *) secondMemory;
    g_RenderBackBufferBase[0] = secondMemory + 0x4080;
    D_800B08F8.ordering[0] = (u32 *)(firstMemory + 0x80);
    g_RenderOtBufferBaseAlt[0] = secondMemory + 0x80;
    Draw_SetPresentEnabled(1);
    result = -1;
    do {
        MemCard_SetDialogActive(1);
        MemCard_InitState();
        SetDispMask(0);
        rect.x = 0x140;
        rect.w = 0xA0;
        rect.y = 0;
        rect.h = 0x100;
        MoveImage(&rect, 0x2C0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        SetDefDrawEnv(&g_MemcardFirstDrawScreen->draw, 0, 0, 0x140, 0xF0);
        SetDefDrawEnv(&g_MemcardSecondScreenView->draw, 0, 0xF0, 0x140, 0xF0);
        SetDefDispEnv(&g_MemcardScreenBuffers[0]->disp, 0, 0xF0, 0x140, 0xF0);
        SetDefDispEnv(&g_MemcardSecondScreen.value->disp, 0, 0, 0x140, 0xF0);
        g_MemcardScreenBuffers[0]->disp.screen.y = g_MemcardSecondScreen.value->disp.screen.y = 0;
        g_MemcardScreenBuffers[0]->disp.screen.h = g_MemcardSecondScreen.value->disp.screen.h = 0xF0;
        g_MemcardScreenBuffers[0]->disp.isrgb24 = g_MemcardSecondScreen.value->disp.isrgb24 = 1;
        g_MemcardScreenBuffers[0]->dirty.w = g_MemcardSecondScreen.value->dirty.w = 0;
        g_MemcardScreenBuffers[0]->overlay.w = g_MemcardSecondScreen.value->overlay.w = 0;
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x1E0;
        rect.h = 0x1E0;
        ClearImage(&rect, 0, 0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        if (titleMode != 0) {
            if (MenuCard_PlayVideo(1) != 0) {
                waitFrames = 0;
                do {
                    waitFrames += 1;
                    MemCard_UpdateSavePolling();
                    VSync(0);
                } while (waitFrames < 0x3C);
            }
            g_MemcardScreenBuffers[0]->draw = g_RenderDrawEnvArray[0];
            g_MemcardSecondScreen.value->draw = g_RenderDrawEnvArray[1];
            g_MemcardScreenBuffers[0]->disp = g_RenderDispEnvArray[0];
            g_MemcardSecondScreen.value->disp = g_RenderDispEnvArray[1];
            g_MemcardScreenBuffers[0]->draw.isbg = 0;
            g_MemcardSecondScreen.value->draw.isbg = 0;
            g_MemcardScreenBuffers[0]->dirty.w = g_MemcardSecondScreen.value->dirty.w = 0;
            g_MemcardScreenPage = g_ActiveDrawSlot;
            g_MemcardScreenBuffers[0]->overlay.w = g_MemcardSecondScreen.value->overlay.w = 0;
            rect.x = 0;
            rect.y = 0;
            rect.w = 0x1E0;
            rect.h = 0x13;
            ClearImage(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 0xDF;
            rect.w = 0x1E0;
            rect.h = 0x11;
            ClearImage(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 0xF0;
            rect.w = 0x1E0;
            rect.h = 0x13;
            ClearImage(&rect, 0, 0, 0);
            rect.x = 0;
            rect.y = 0x1CF;
            rect.w = 0x1E0;
            rect.h = 0x11;
            ClearImage(&rect, 0, 0, 0);
            initialPage = g_MemcardScreenPage == 0;
            g_MemcardScreenPage = initialPage;
            g_MemcardCurrentScreen = g_MemcardScreenBuffers[initialPage];
            DrawSync(0);
            if (g_MemcardCurrentScreen->overlay.w > 0) {
                overlay = g_MemcardCurrentScreen->overlay;
                overlay.x = (s16) ((overlay.x * 3) >> 1);
                if (g_MemcardScreenPage == 0) {
                    overlay.y += 0xF0;
                }
                overlay.w = (s16) ((overlay.w * 3) >> 1);
                LoadImage(&overlay, g_MemcardCurrentScreen->overlayImage);
            }
            VSync(0);
            ResetGraph(1);
            PutDrawEnv(&g_MemcardCurrentScreen->draw);
            PutDispEnv(&g_MemcardCurrentScreen->disp);
        }
        func_8018F2F4();
        VSync(0);
        SetDispMask(1);
        /* Rebuild the eight-node free list for this menu pass. */
        freeNode = g_MemcardImageNodePool;
        freeEnd = &g_MemcardImageNodePool[7];
        if ((u32) g_MemcardImageNodePool < (u32) freeEnd) {
            do {
                link = &freeNode[1];
                freeNode->next = link;
                freeNode = link;
            } while ((u32) freeNode < (u32) freeEnd);
        }
        freeNode->next = 0;
        g_MemcardFreeImageNodes = g_MemcardImageNodePool;
        g_MemcardPendingNodeTail = 0;
        g_MemcardPendingNodeHead = 0;
        g_MemcardActiveNodeTail = 0;
        g_MemcardActiveNodeHead = 0;
        MenuCard_CreateImageNode(1)->field14 = g_MenuCardCreateFadeImageNodeEntry;
        g_MemcardScreenTimer = titleMode;
        if (titleMode < 0x3E8) {
            do {
                page = g_MemcardScreenPage == 0;
                g_MemcardScreenPage = page;
                g_MemcardCurrentScreen = g_MemcardScreenBuffers[page];
                MemCard_UpdateSavePolling();
                Field_HandleStateTransition();
                MenuCard_HandleListInput();
                MenuCard_RedrawImageNodes();
                node = (g_MemcardActiveNodeHead);
                previous = 0;
                if (node != 0) {
                    do {
                        if (node->freePending != 0) {
                            if (previous != 0) {
                                previous->next = node->next;
                            } else {
                                g_MemcardActiveNodeHead = node->next;
                            }
                            if (g_MemcardActiveNodeTail == node) {
                                g_MemcardActiveNodeTail = previous;
                            }
                            link = g_MemcardFreeImageNodes;
                            g_MemcardFreeImageNodes = node;
                            node->next = link;
                            if (previous != 0) {
                                node = previous->next;
                            } else {
                                node = (g_MemcardActiveNodeHead);
                            }
                        } else {
                            previous = node;
                            node = node->next;
                        }
                    } while (node != 0);
                }
                {
                    MemcardImageNode *pending = g_MemcardPendingNodeHead;
                    if (pending != 0) {
                        MemcardImageNode *tail = g_MemcardActiveNodeTail;

                        if (tail != 0) {
                            link = g_MemcardPendingNodeTail;
                            tail->next = pending;
                        } else {
                            link = g_MemcardPendingNodeTail;
                            g_MemcardActiveNodeHead = pending;
                        }
                        g_MemcardActiveNodeTail = link;
                        g_MemcardPendingNodeTail = 0;
                        g_MemcardPendingNodeHead = 0;
                    }
                }
                DrawSync(0);
                if (g_MemcardCurrentScreen->overlay.w > 0) {
                    overlay = g_MemcardCurrentScreen->overlay;
                    overlay.x = (s16) ((overlay.x * 3) >> 1);
                    if (g_MemcardScreenPage == 0) {
                        overlay.y += 0xF0;
                    }
                    overlay.w = (s16) ((overlay.w * 3) >> 1);
                    LoadImage(&overlay, g_MemcardCurrentScreen->overlayImage);
                }
                VSync(2);
                ResetGraph(1);
                PutDrawEnv(&g_MemcardCurrentScreen->draw);
                PutDispEnv(&g_MemcardCurrentScreen->disp);
                elapsed = g_MemcardScreenTimer + titleMode;
                g_MemcardScreenTimer = elapsed;
            } while (elapsed < 0x3E8);
        }
        SetDispMask(0);
        rect.x = 0x2C0;
        rect.w = 0xA0;
        rect.y = 0;
        rect.h = 0x100;
        MoveImage(&rect, 0x140, 0);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x140;
        rect.h = 0x1E0;
        ClearImage(&rect, 0, 0, 0);
        DrawSync(0);
        VSync(0);
        VSync(0);
        screenYRestore = 8;
        Draw_InitBuffers();
        Draw_SelectBuffer();
        Draw_PresentFrame(2);
        if (g_MemcardScreenTimer >= 0x3E9) {
            wantedKind = 7;
            selection = (g_MemcardActiveNodeHead);
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
                GameTime_InitCounter2();
                GameTime_InitCounter0();
            } else if ((selectionY >= 0xB5) || ((selectionY == 0xB4) && (MemCard_IsPortPresent(0) == 0) && (MemCard_IsPortPresent(1) == 0))) {
                result = 3;
            } else {
                Draw_SetPresentImage((u8 *)g_MemcardSecondScreenView + 0x1C080);
                image = (MemcardImage *)&g_MemcardImageBundle[*g_MemcardImageOffsets];
                source = (u32 *)(image + 1);
                groupsRemaining = (image->width * image->height) / 6;
                destination = (u32 *)((u8 *)g_MemcardSecondScreenView + 0x1C080);
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
                Menu_OpenMemCardSelectDialog(0);
                Save_SetTitleStyleFlag(selectionY == 0x8C);
                do {
                    Field_HandleStateTransition();
                    result = Menu_RunFrameWithArg(0);
                } while (result == 0);
                if ((result == 2) && (selectionY == 0x8C)) {
                    GameTime_InitCounter2();
                }
                SetDispMask(0);
                Draw_SetPresentImage(0);
                rect.x = 0;
                rect.y = 0;
                rect.w = 0x140;
                rect.h = 0x1E0;
                ClearImage(&rect, 0, 0, 0);
                DrawSync(0);
                screenYRestore = g_RenderDispEnvArray[0].screen.y;
            }
        }
    } while (result < 0);
    g_RenderDrawEnvArray[0] = g_MemcardListSavedDrawEnv[0];
    g_RenderDrawEnvArray[1] = g_MemcardListSavedDrawEnv[1];
    g_RenderDispEnvArray[0] = g_MemcardListSavedDispEnv[0];
    g_RenderDispEnvArray[1] = g_MemcardListSavedDispEnv[1];
    {
        /* Color processing is complete; reuse its scratch for display Y. */
        packedColor = screenYRestore;
        g_RenderDispEnvArray[1].screen.y = packedColor;
        g_RenderDispEnvArray[0].screen.y = packedColor;
    }
    MemCard_SetDialogActive(0);
    Draw_SetPresentEnabled(0);
    return result;
}
