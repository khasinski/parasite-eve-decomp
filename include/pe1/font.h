#ifndef PE1_FONT_H
#define PE1_FONT_H

#include "common.h"

/* Partial glyph-table layout recovered from the slot lookup at 0x80038BC0.
 * The first list begins at +4; the second list's count is at +0x1C.
 * Names describe observed lookups, not an identified original file format. */
typedef struct FontGlyphGroups {
    unsigned char unknown[2];
    unsigned char count;
    unsigned char codes[24];
} FontGlyphGroups;

typedef struct FontGlyphSlots {
    unsigned char count;
    unsigned char indices[100];
    unsigned char groupKeys[100];
    unsigned char unknownC9[100];
} FontGlyphSlots;

typedef struct FontGlyphTable {
    unsigned char unknown;
    FontGlyphGroups groups;
    FontGlyphSlots slots;
} FontGlyphTable;

/* The disc-load view begins at the PE.IMG base LBA. Its destination pointer
 * is also read through D_800B0E6C while copying the selected glyph table. */
typedef struct FontGlyphLoadState {
    u32 baseLba;
    u8 unknown04[0x90];
    u8 *buffer;
} FontGlyphLoadState;

/* The selection byte is nine bytes before the table pointer. Navigation
 * derives its address from the pointer member's address in retail code. */
typedef struct FontGlyphSelectionState {
    unsigned char unknown00[3];
    unsigned char selected;
    unsigned char unknown04[8];
    FontGlyphTable *table;
} FontGlyphSelectionState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(FontGlyphTable, groups.codes) == 4,
                  font_group_codes_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FontGlyphTable, slots) == 0x1C,
                  font_slots_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FontGlyphSlots, groupKeys) == 0x65,
                  font_slot_group_keys_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FontGlyphSelectionState, selected) == 3,
                  font_selection_index_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FontGlyphSelectionState, table) == 0xC,
                  font_selection_table_offset);
PE1_STATIC_ASSERT(sizeof(FontGlyphSelectionState) == 0x10,
                  font_selection_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FontGlyphLoadState, buffer) == 0x94,
                  font_glyph_load_buffer_offset);

extern FontGlyphSelectionState g_FontSelectionState;
extern FontGlyphTable *D_80091A28;
extern unsigned char D_80091A1D;
extern FontGlyphTable D_8009ECD8;
extern FontGlyphLoadState D_800B0DD8;
extern u8 *D_800B0E6C;
extern u16 D_80093176[2];

/* Historical name: returns a slot index, or 0xFF; performs no drawing.
 * A null table selects D_80091A28. */
unsigned char Render_DrawTextDigit(FontGlyphTable *table, unsigned char mode);

unsigned char Render_GetOrLoadFontGlyph(unsigned char action);
unsigned char Render_FindFontGlyphSlot(void);
unsigned char Render_StepFontLoad(void);

extern unsigned char D_80091A1E, D_80091A1F, D_80091A20;
extern unsigned char D_8009EE22[];
int Render_LoadFontGlyph(unsigned char code);
unsigned char Render_SetFontGlyphByCode(unsigned char code);

#endif
