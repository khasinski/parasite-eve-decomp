/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
/* Field render-flag table setup and the helper that stores one table entry
 * at the bit index of a single-bit mask. The helper's only caller is the
 * table builder that precedes it (main_tu_evidence G0344). */
#include "common.h"
#include "pe1/gte.h"

extern s32 D_8009D1A0r[] __asm__("g_GameStateFlags");
extern s32 D_8009D1A0w[] __asm__("g_GameStateFlags");
extern s32 g_SceneFlagBits;
extern s32 g_BattleControlFlags;
extern s32 g_FieldRenderFlags;
extern s32 g_FieldPadBits;
extern s32 D_8009D2D4;
extern s32 g_FieldRenderFlagTable[];
extern int D_800A76F0[];

void Gte_StoreTableEntry(u32 mask, int value);

void Boot_BuildRenderFlagTable(void) {
    s32 *entry;
    u32 i;

    i = 0;
    entry = g_FieldRenderFlagTable;
    g_SceneFlagBits = 0;
    g_BattleControlFlags = 0;
    D_8009D2D4 = 0;
    g_FieldRenderFlags = 0;
    g_FieldPadBits = 0;
    do {
        *entry = 0;
        i += 1;
        entry += 1;
    } while (i < 0x20U);
    Gte_StoreTableEntry(1, 0x4000);
    Gte_StoreTableEntry(0x80, 0x1000);
    Gte_StoreTableEntry(0x100, 0x2000);
    Gte_StoreTableEntry(8, 0x10);
    Gte_StoreTableEntry(0x20, 0x40);
    Gte_StoreTableEntry(0x40, 0x80);
    Gte_StoreTableEntry(0x10, 0x20);
    Gte_StoreTableEntry(2, 1);
    Gte_StoreTableEntry(4, 8);
    Gte_StoreTableEntry(0x200, 0x2000);
    Gte_StoreTableEntry(0x400, 0x4000);
    Gte_StoreTableEntry(0x2000, 0x8000);
    Gte_StoreTableEntry(0x01000000, 0x100);
    Gte_StoreTableEntry(0x02000000, 0x200);
    Gte_StoreTableEntry(0x04000000, 0x400);
    Gte_StoreTableEntry(0x08000000, 0x800);
    Gte_StoreTableEntry(0x10000000, 0x1000);
    Gte_StoreTableEntry(0x20000000, 0x2000);
    Gte_StoreTableEntry(0x40000000, 0x4000);
    Gte_StoreTableEntry(0x80000000, 0x8000);
    D_8009D1A0w[0] = D_8009D1A0r[0] | 0x4000;
}

void Gte_StoreTableEntry(u32 mask, int value) {
    int index;

    gte_ldlzcs(mask);
    index = 31;
    if (mask != 0x80000000) {
        gte_stlzcr(&mask);
        index -= mask;
    }
    D_800A76F0[index] = value;
}
