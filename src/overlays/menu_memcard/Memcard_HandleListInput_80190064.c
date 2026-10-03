#include "menu_memcard_image.h"

static inline MemcardImageNode *Memcard_FindImageNode(s32 kind) {
    MemcardImageNode *node;

    for (node = D_801D1370; node != 0; node = node->next) {
        if (node->kind == kind) {
            break;
        }
    }
    return node;
}

/* Scroll and confirm handling for the memory card list screen. */
void func_80190064(void) {
    s32 pad;
    s32 y;
    MemcardImageNode *node;

    pad = func_8005E038();
    if ((pad & 0x800) && !(D_801D11B8 & 0x800)) {
        node = Memcard_FindImageNode(2);
        if (node != 0 && node->field1c > 0x80) {
            node = Memcard_FindImageNode(1);
            if (node->stepY == 0) {
                node->stepY = 1;
                node->parameter = 8;
                node->field14 = D_801931BC;
                D_801D1380 = 0;
                Menu_PlayConfirmSound();
            }
        }
    }
    node = Memcard_FindImageNode(5);
    if (node != 0 && node->draw == 0) {
        if (func_80042770(0) != 0 || func_80042770(1) != 0) {
            if (node->y < 200) {
                node->y++;
                Memcard_FindImageNode(4)->targetY = 16;
                D_801D1380 = 0;
            }
            if (node->y == 200 && func_8003FFCC() != 0) {
                node = Memcard_FindImageNode(6);
                if (node->targetY == 0) {
                    node->draw = D_801930D8;
                    node->update = Memcard_BlendRotatedImage;
                    node->targetY = 20;
                    node->stepY = 1;
                }
            }
        } else if (node->y >= 181) {
            node->y--;
            if (Memcard_FindImageNode(7)->y > node->y) {
                Memcard_FindImageNode(7)->y = node->y;
            }
            Memcard_FindImageNode(4)->targetY = -16;
            D_801D1380 = 0;
        }
        if (func_8003FFCC() == 0) {
            node = Memcard_FindImageNode(6);
            if (node->targetY == 0x54) {
                node->draw = D_8019316C;
            }
            if (node->field1c != 0) {
                node->stepY = -1;
                if (Memcard_FindImageNode(7)->y == 140) {
                    Memcard_FindImageNode(7)->y = 160;
                }
            }
        }
        node = Memcard_FindImageNode(7);
        if (pad & 0x20) {
            if (node->y == 160 || node->y == 180 || node->y == 200 || node->y == 140) {
                D_801D1380 = 1001;
                Menu_PlayConfirmSound();
            }
        }
        if ((pad & 0x1000) && !(D_801D11B8 & 0x1000)) {
            y = node->y;
            if (Memcard_FindImageNode(6)->field1c == 0x100 ? y >= 141 : y >= 161) {
                D_801D1380 = 0;
                node->y -= 20;
                Menu_PlayMoveSound();
            }
        }
        if ((pad & 0x4000) && !(D_801D11B8 & 0x4000)) {
            y = node->y;
            if (y <= Memcard_FindImageNode(5)->y - 20) {
                D_801D1380 = 0;
                node->y += 20;
                Menu_PlayMoveSound();
            }
        }
    }
    D_801D11B8 = pad;
}
