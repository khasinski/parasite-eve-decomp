/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Render_LoadFontGlyph();
extern struct { char _[16]; } g_MenuEquipSlotState_o __asm__("g_MenuEquipSlotState");
#define g_MenuEquipSlotState (*(u8 *)&g_MenuEquipSlotState_o)
extern struct { char _[16]; } D_80091A1E_o __asm__("g_FontGlyphCode");
#define g_FontGlyphCode (*(u8 *)&D_80091A1E_o)
extern struct { char _[16]; } D_80091A1F_o __asm__("g_FontGlyphIndex");
#define g_FontGlyphIndex (*(u8 *)&D_80091A1F_o)
extern u8 D_80091A1F_rd[] __asm__("g_FontGlyphIndex");
extern struct { char _[16]; } D_80091A20_o __asm__("g_FontGlyphLoadFailed");
#define g_FontGlyphLoadFailed (*(s8 *)&D_80091A20_o)
extern struct { char _[16]; } D_80091A28_o __asm__("g_FontGlyphTable");
#define g_FontGlyphTable (*(void **)&D_80091A28_o)
extern struct { char _[16]; } D_8009EE22_o __asm__("g_FontGlyphCodeTable");
#define g_FontGlyphCodeTable (*(M2C_UNK *)&D_8009EE22_o)

u8 Render_FindFontGlyphSlot(void) {
    u8 temp_v0;
    u8 temp_a0;
    u8 *hdr;
    u8 *p;
    u8 *p2;
    u8 *q;
    u8 *t;
    s32 i;
    s32 found;
    s32 slot;

    if ((u8) g_MenuEquipSlotState >= 0x46U) {
        g_FontGlyphLoadFailed = 1;
        return 0xFFU;
    }
    temp_v0 = g_MenuEquipSlotState + 1;
    g_MenuEquipSlotState = temp_v0;
    temp_a0 = *((u8 *)&D_8009EE22_o + temp_v0);
    g_FontGlyphLoadFailed = 0;
    g_FontGlyphCode = temp_a0;
    Render_LoadFontGlyph(temp_a0);
    found = 0;
    hdr = g_FontGlyphTable;
    p = hdr + 1;
    for (i = 0; i < hdr[3]; i++) {
        q = p + i;
        if (q[3] == 2) {
            found = i;
            i = p[2];
        }
    }
    p2 = p + 0x1B;
    for (i = 0; i < p[0x1B]; i++) {
        q = p2 + i;
        if (q[1] == (found & 0xFF)) {
            slot = i;
            goto store;
        }
    }
    slot = 0xFF;
store:
    g_FontGlyphIndex = slot;
    __asm__ volatile("" : : : "memory");
    t = g_FontGlyphTable;
    return *(t + *(t + D_80091A1F_rd[0] + 0x1D) + 4);
}
