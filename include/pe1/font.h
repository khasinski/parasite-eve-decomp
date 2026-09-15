#ifndef PE1_FONT_H
#define PE1_FONT_H

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
    unsigned char indices[0]; /* Variable-length trailing list. */
} FontGlyphSlots;

typedef struct FontGlyphTable {
    unsigned char unknown;
    FontGlyphGroups groups;
    FontGlyphSlots slots;
} FontGlyphTable;

extern FontGlyphTable *D_80091A28;
extern unsigned char D_80091A1D;

/* Historical name: returns a slot index, or 0xFF; performs no drawing.
 * A null table selects D_80091A28. */
unsigned char Render_DrawTextDigit(FontGlyphTable *table, unsigned char mode);

#endif
