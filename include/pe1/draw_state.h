#ifndef PE1_DRAW_STATE_H
#define PE1_DRAW_STATE_H

extern unsigned char *g_DrawPacketCursor __asm__("D_8009D100");
extern unsigned char *g_DrawPacketArenaBase __asm__("D_8009D104");
extern int g_DrawColorSelect __asm__("D_8009D10C");
extern int g_DrawPrimaryColor __asm__("D_8009D110");
extern int g_DrawAlternateColor __asm__("D_8009D114");
extern unsigned int *g_DrawOrderingTableEntry __asm__("D_8009D11C");
extern int *g_TextCursorStack __asm__("D_8009D12C");

#endif /* PE1_DRAW_STATE_H */
