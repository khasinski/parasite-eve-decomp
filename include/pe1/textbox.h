#ifndef PE1_TEXTBOX_H
#define PE1_TEXTBOX_H

/* Textbox / on-screen dialogue entry, walked by Menu_DrawTextboxEntries.
 * g_TextboxEntries (= D_800BCEA8) is an array of these, stride 0x38. Each
 * frame the engine walks the active entries and types out / draws their text.
 * See include/pe1/text.h for the surrounding text subsystem notes. */

typedef struct TextboxEntry {
    /* 0x00 */ unsigned char state;          /* 0=free, 1=new -> init, 2=active/displaying */
    /* 0x01 */ unsigned char pad_01[0x03];
    /* 0x04 */ unsigned char *message;       /* live typewriter position into the message blob */
    /* 0x08 */ unsigned char style;          /* box style (0/1/3) */
    /* 0x09 */ unsigned char pad_09[0x03];
    /* 0x0C */ int flags;
    /* 0x10 */ short page_id;
    /* 0x12 */ unsigned short x;
    /* 0x14 */ unsigned short y;
    /* 0x16 */ unsigned short color0;
    /* 0x18 */ unsigned short color1;
    /* 0x1A */ unsigned char pad_1A[0x1E];   /* inline data through end of entry */
} TextboxEntry;                              /* 0x38 */

extern TextboxEntry g_TextboxEntries[];      /* = D_800BCEA8 */

#endif /* PE1_TEXTBOX_H */
