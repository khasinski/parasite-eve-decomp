#ifndef PE1_TEXT_H
#define PE1_TEXT_H

#include "common.h"

/* A counted table of entries addressed by signed byte offsets relative to the
 * start of the table itself; `bytes` resolves an offset. */
typedef union TextOffsetTable {
    struct {
        u16 count;
        s16 offsets[0];
    } header;
    u8 bytes[0];
} TextOffsetTable;

/* Table offsets are byte offsets relative to this directory; the first word
 * is unknown. */
typedef union TextTableDirectory {
    struct {
        u32 reserved;
        s32 tableOffsets[4];
    } header;
    u8 bytes[0];
} TextTableDirectory;

void *Text_LookupTable4(unsigned int index);
void *Text_LookupTable8(unsigned int index);
extern int g_SaveMetadataWindowIndex;
extern int g_TextTableSet;
void Text_CopyFFTerminatedBytes(u8 *dst, u8 *src);
void Text_AppendFFTerminatedBytes(u8 *dst, u8 *src);
int Draw_MeasureTextWidth(u8 *text);
void Draw_OffsetCursor(int x, int y);
void Draw_PrintCenteredText(u8 *text);
void Draw_PrintTextById(unsigned int textId);

/* Text / dialogue rendering subsystem (message boxes, menu labels, numbers).
 * Verified from code + live DuckStation GDB (talked to an NPC on the first
 * field screen).
 *
 * STRING FORMAT: a byte array, each byte a GLYPH CODE, terminated by 0xFF.
 * (Not ASCII -- codes index the game's image font; control/spacing codes are
 *  specific low byte values handled while emitting; 0xFF = end of string.)
 *
 * RENDER PATH: every printer loops bytes until 0xFF, calling
 *   Draw_AllocTexturedQuad(code) -> one textured quad sampling the glyph from
 *   the VRAM font cache. Printers:
 *     Draw_PrintRawText(u8 *s)              -- raw string
 *     Draw_PrintTextById(index)             -- string from a table (Text_LookupTable4)
 *     Draw_PrintTextWrapped(u8 *s, int w)   -- word-wrap to width w
 *     Draw_PrintCenteredText / *InWidth     -- centered
 *     Draw_PrintNumberWidth2..6 / Signed*   -- numeric fields
 *     Draw_SetFontVariant / Draw_SetTextDimmed / Draw_SetGlyphAdvance -- style
 *     Draw_MeasureTextWidth(s)              -- measure without drawing
 *   Each printer pushes/pops the text cursor on a small stack so nested prints
 *   restore position.
 *
 * TEXT CURSOR (pen position):
 *   g_TextCursorX (0x8009D124)  cursor X (live: 8 inside the dialogue box)
 *   g_TextCursorY (0x8009D128)  cursor Y (live: 32)
 *   g_TextCursorStackPtr  cursor stack pointer; bounds g_TextCursorStackBottom .. g_TextCursorStackTop
 *               (top). Printers push {X,Y} (8 bytes) then pop to restore.
 *   g_TextRenderMode  text mode/variant flag read by the quad allocator.
 *
 * FONT (image glyphs, not a vector/ASCII font):
 *   Glyph bitmaps are streamed from PE.IMG by code (Render_LoadFontGlyph /
 *   Render_StepFontLoad, via CdRom_ReadSectorsFromLba + g_PeImageBaseLba) and
 *   cached in VRAM. Lookup/alloc: Render_GetOrLoadFontGlyph,
 *   Render_FindFontGlyphSlot, Render_SetFontGlyphByCode; the glyph cache state
 *   is the D_8009ECxx..D_8009EDxx block. Draw_AllocTexturedQuad maps a code to
 *   its cached glyph's TPAGE/UV and emits the quad; Draw_LookupGlyphDescriptor /
 *   Draw_LookupGlyphMetrics / Draw_EmitGlyph back the metrics/advance.
 *
 * MESSAGE SOURCE: messages are looked up by index from PE asset tables (loaded
 * from PE.IMG): Text_LookupTable0/4/8/C/10 (e.g. Text_LookupTable4 at 0x8005DC4C,
 * used by Draw_PrintTextById and many Menu_Draw* helpers). The string returned
 * is the 0xFF-terminated glyph-code array above.
 *
 * TEXTBOX ENGINE (field dialogue / on-screen messages) -- Menu_DrawTextboxEntries:
 *   g_TextboxEntries = array of textbox ENTRY structs, stride 0x38. Each frame the
 *   engine walks the active entries and types out / draws their text. Entry:
 *     +0x00 u8   state (0=free, 1=new -> init, 2=active/displaying)
 *     +0x04 u8*  CURRENT message pointer == the live typewriter position; for a
 *                new entry it is seeded from g_TextboxMessageSource then advanced past
 *                control codes. This points into the room/event message blob
 *                (loaded with the scene, 0x801Axxxx region).
 *     +0x08 u8   box style (0/1/3), +0x12/+0x14 u16 x/y, +0x16/+0x18 colour,
 *     +0x0C flags, +0x10 page/id, +0x1A.. inline data.
 *   Control codes in the stream: 0xF7 (switch/colour), 0xF8/0xF9, 0xFE <arg>
 *   (page/cond), 0xFF (end). Speaker is in-line, e.g. "Doorman" + ':' + text.
 *   g_TextboxMessageSource holds the message SOURCE handed to a freshly-queued entry (the
 *   "show message" call); after that the per-entry +0x04 is the live cursor.
 *
 *   *** To know the current on-screen text: scan g_TextboxEntries (0x38 stride), and
 *   for each entry with state != 0 decode the 0xFF-terminated string at +0x04.
 *   (Verified live: entry[0] state=2, +0x04 -> "Doorman: Enjoy the show".) ***
 *
 * Quick live probe: text cursor = *(s32*)0x8009D124 (x), *(s32*)0x8009D128 (y);
 *   current message = decode *(u8**)(0x800BCEA8 + 0x04) (entry 0).
 */

/* Nth non-empty entry of the location name table (text/tables.c). */
u8 *Text_FindNthNonEmpty(int index);

#endif /* PE1_TEXT_H */
