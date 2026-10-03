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
