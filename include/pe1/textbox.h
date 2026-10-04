#ifndef PE1_TEXTBOX_H
#define PE1_TEXTBOX_H

#include "pe1/render_packets.h"

/* Textbox / on-screen dialogue entry, walked by Menu_DrawTextboxEntries.
 * g_TextboxEntries (= D_800BCEA8) is an array of these, stride 0x38. Each
 * frame the engine walks the active entries and types out / draws their text.
 * See include/pe1/text.h for the surrounding text subsystem notes. */

typedef struct TextboxNumber {
    u8 digits[5];
    s8 count;
} TextboxNumber;
typedef union TextboxControl {
    s32 flags;
    struct {
        u8 delay, elapsed;
        u16 bits;
    } parts;
} TextboxControl;
typedef struct TextboxEntry {
    u8 state;
    u8 pad01[3];
    u8 *message;
    u8 style, background;
    u8 pad0A[2];
    TextboxControl control;
    s16 page_id;
    u16 x, y, width, height;
    TextboxNumber numbers[5];
} TextboxEntry;

typedef struct TextboxFontPage {
    u32 reserved;
    u16 textureX, textureY, paletteX, paletteY;
    u16 tpage, clut;
} TextboxFontPage;

typedef struct TextboxNameGlyphs {
    u8 glyphs[9];
    s8 count;
} TextboxNameGlyphs;
typedef struct TextboxGlyphSpacing {
    u8 left, right;
} TextboxGlyphSpacing;
typedef struct TextboxPoint {
    u16 x, y;
} TextboxPoint;
typedef struct TextboxGlyphPacket {
    u32 tag, drawMode;
    RenderSpritePacket sprite;
} TextboxGlyphPacket;

extern TextboxEntry g_TextboxEntries[]; /* = D_800BCEA8 */

PE1_STATIC_ASSERT(sizeof(TextboxEntry) == 0x38, textbox_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TextboxEntry, control) == 0xC, textbox_control_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TextboxEntry, numbers) == 0x1A, textbox_numbers_offset);
PE1_STATIC_ASSERT(sizeof(TextboxNumber) == 6, textbox_number_size);
PE1_STATIC_ASSERT(sizeof(TextboxGlyphPacket) == 28, textbox_glyph_packet_size);

void Menu_DrawTextboxEntries(void);
void Tbl_ResetAll(void);
void Menu_SetTextCursorRect(int x, int y, int width, int height);
void Render_SetupColorTable(int index, int mode, short *colors);
void Draw_SetCursor(int x, int y);
void Draw_AllocColorGradient(int width, int height, int arg2, int arg3);

#endif /* PE1_TEXTBOX_H */
