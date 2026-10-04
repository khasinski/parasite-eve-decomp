#ifndef PE1_FIELD_BG_LOAD_H
#define PE1_FIELD_BG_LOAD_H

#include "common.h"
#include "pe1/psyq_gpu.h"

/* Background texture slots: VRAM page origin and CLUT origin, with the
 * packed tpage/clut words the loader derives from them. */
typedef struct FieldBgTextureSlot {
    u16 x, y;
    u16 clut_x, clut_y;
    u16 tpage, clut;
    u32 reserved;
} FieldBgTextureSlot;

/* Raw image upload record: total length in bytes, VRAM rectangle, pixels. */
typedef struct FieldBgImageRecord {
    u32 length;
    RECT rect;
    u32 pixels[1];
} FieldBgImageRecord;

/* libgpu getTPage/getClut packing (texture mode 0, semi-transparency 1). */
#define BG_SLOT_TPAGE(x, y) \
    ((0 << 7) | (1 << 5) | (((y) & 0x100) >> 4) | (((x) & 0x3ff) >> 6) | (((y) & 0x200) << 2))
#define BG_SLOT_CLUT(x, y) (((y) << 6) | (((x) >> 4) & 0x3f))

extern FieldBgTextureSlot D_80091648[4];

/* PE.IMG start/end sector table (consecutive entries bound one chunk). */
extern u16 D_800930D8[];

void Akao_Cmd_F1(void);
void DrawSync(int mode);
void Render_InitEntityPool(int mode);
void SetDispMask(int mask);
void Battle_DrawHPBar(void);

int Scene_LoadFieldBg(void);

#endif
