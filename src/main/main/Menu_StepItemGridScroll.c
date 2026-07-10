typedef signed short s16;
typedef unsigned char u8;

#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define STAT_TOTAL(base, stat_off, mod_off) \
    ((U8((base), (stat_off)) + S16((base), (mod_off)) < 1000) ? \
        (U8((base), (stat_off)) + S16((base), (mod_off))) : 999)

extern int D_8009CF18;
extern int D_8009CF1C;

void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int glyph);
void Draw_PrintSignedNumberWidth4(int value);
void Draw_PrintNumberWidth4(int value);
void Draw_SetStatCompareColor(int current, int compare);
void Draw_SetColor(int color);

void Menu_StepItemGridScroll(void *current, void *compare) {
    int sprite_base;
    int current_value;
    int compare_value;

    if (compare == 0) {
        return;
    }

    if (current != 0) {
        if (D_8009CF1C != 0) {
            Draw_OffsetCursor(0x28, -0x32);
            Draw_AllocSprite(0x88);
            Draw_OffsetCursor(0, 0xA);
            Draw_PrintSignedNumberWidth4(S16(current, 0xE));
            Draw_OffsetCursor(-0x14, 0xE);
            Draw_PrintSignedNumberWidth4(S16(current, 0x10));
            Draw_OffsetCursor(-0x14, 0xE);
            Draw_PrintSignedNumberWidth4(S16(current, 0x12));
            Draw_OffsetCursor(-0x26, 0xC);
        }

        Draw_OffsetCursor(0x26, -0xC);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);

        if (D_8009CF1C != 0) {
            Draw_OffsetCursor(0xC, -0xA);
            Draw_AllocSprite(0x88);

            Draw_SetStatCompareColor(S16(current, 0xE), S16(compare, 0xE));
            Draw_OffsetCursor(0, 0xA);
            Draw_PrintSignedNumberWidth4(S16(compare, 0xE));

            Draw_OffsetCursor(-0x14, 0xE);
            Draw_SetStatCompareColor(S16(current, 0x10), S16(compare, 0x10));
            Draw_PrintSignedNumberWidth4(S16(compare, 0x10));

            Draw_OffsetCursor(-0x14, 0xE);
            Draw_SetStatCompareColor(S16(current, 0x12), S16(compare, 0x12));
            Draw_PrintSignedNumberWidth4(S16(compare, 0x12));
        } else {
            current_value = STAT_TOTAL(current, 7, 0xE);
            compare_value = STAT_TOTAL(compare, 7, 0xE);
            Draw_SetStatCompareColor(current_value, compare_value);
            Draw_OffsetCursor(0xC, -2);
            Draw_PrintNumberWidth4(compare_value);

            Draw_OffsetCursor(-0x24, 0xE);
            current_value = STAT_TOTAL(current, 8, 0x10);
            compare_value = STAT_TOTAL(compare, 8, 0x10);
            Draw_SetStatCompareColor(current_value, compare_value);
            Draw_PrintNumberWidth4(compare_value);

            Draw_OffsetCursor(-0x24, 0xE);
            current_value = STAT_TOTAL(current, 9, 0x12);
            compare_value = STAT_TOTAL(compare, 9, 0x12);
            Draw_SetStatCompareColor(current_value, compare_value);
            Draw_PrintNumberWidth4(compare_value);
        }

        Draw_SetColor(0x808080);
        return;
    }

    sprite_base = 0x7F;
    if (D_8009CF18 != 0) {
        sprite_base = 0x7C;
    }

    Draw_OffsetCursor(4, 0x17);
    Draw_AllocSprite(sprite_base);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 1);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 2);
    Draw_SetColor(0x8080);

    Draw_OffsetCursor(0x28, -0x1C);
    compare_value = STAT_TOTAL(compare, 7, 0xE);
    Draw_SetStatCompareColor(0, compare_value);
    Draw_PrintNumberWidth4(compare_value);

    Draw_OffsetCursor(-0x24, 0xE);
    compare_value = STAT_TOTAL(compare, 8, 0x10);
    Draw_SetStatCompareColor(0, compare_value);
    Draw_PrintNumberWidth4(compare_value);

    Draw_OffsetCursor(-0x24, 0xE);
    compare_value = STAT_TOTAL(compare, 9, 0x12);
    Draw_SetStatCompareColor(0, compare_value);
    Draw_PrintNumberWidth4(compare_value);

    Draw_SetColor(0x808080);
}
