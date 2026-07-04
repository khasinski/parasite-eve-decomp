/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Gte_StoreTableEntry();
extern s32 D_8009D1A0r[] __asm__("g_GameStateFlags");
extern s32 D_8009D1A0w[] __asm__("g_GameStateFlags");
extern s32 g_SceneFlagBits;
extern s32 g_BattleControlFlags;
extern s32 g_FieldRenderFlags;
extern s32 g_FieldPadBits;
extern s32 D_8009D2D4;
extern s32 g_FieldRenderFlagTable[];
#define g_FieldRenderFlagTable (g_FieldRenderFlagTable[0])

void Boot_BuildRenderFlagTable(void) {
    s32 *var_v1;
    u32 var_a0;

    var_a0 = 0;
    var_v1 = &g_FieldRenderFlagTable;
    g_SceneFlagBits = 0;
    g_BattleControlFlags = 0;
    D_8009D2D4 = 0;
    g_FieldRenderFlags = 0;
    g_FieldPadBits = 0;
    do {
        *var_v1 = 0;
        var_a0 += 1;
        var_v1 += 1;
    } while (var_a0 < 0x20U);
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
