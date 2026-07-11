/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

#define U16(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define S16(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))

void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_AllocSprite(int glyph);
void Draw_OffsetCursor(int x, int y);
void Draw_PrintRawText(u8 *text);
void Draw_PrintNumberWidth3Unk(int value);

void Sfx_DrawSlotRow(u8 *entry, u8 *text) {
    int type;
    int sprite;

    Draw_StatePush();

    if (entry[6] < 8) {
        sprite = entry[9] + S16(entry, 0x12);
        if (sprite < 1000) {
            if (U16(entry, 0xA) == sprite) {
                Draw_AllocSprite(0x69);
            }
        } else if (U16(entry, 0xA) == 999) {
            Draw_AllocSprite(0x69);
        }
    }

    Draw_AllocSprite(entry[0] - 1);
    Draw_OffsetCursor(0x12, 0);

    if (text != 0) {
        Draw_PrintRawText(text);
    }

    type = entry[6];
    if (type != 0 && type < 8) {
        if (type - 4 <= 0) {
            goto draw_ammo;
        }
        if (type == 4) {
            goto done;
        }
    } else if (type < 0x13 || type == 0x12) {
        goto done;
    }

draw_ammo:
    Draw_OffsetCursor(0x56, 0);

    type = entry[6];
    if (type != 0 && type < 8) {
        if (type - 4 > 0) {
            sprite = type + 0x1A;
        } else {
            sprite = 0x1F;
        }
    } else if (type < 0x13) {
        sprite = 0x1E;
    } else {
        sprite = type + 0xC;
    }

    Draw_AllocSprite(sprite);
    Draw_OffsetCursor(0, 5);
    Draw_PrintNumberWidth3Unk(U16(entry, 0xA));

done:
    Draw_StatePop();
}
