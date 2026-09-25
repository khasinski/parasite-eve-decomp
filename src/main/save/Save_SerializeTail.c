#include "common.h"

#include "pe1/save_blob.h"

extern SaveBytes800 g_EntityWorkBuffer __asm__("D_800A77F0");
extern SaveBytes70 g_AyaBattleState __asm__("D_800B8A20");
extern SaveBytes18 g_SavedBattleStateTail __asm__("D_800B0CB0");
extern SaveBytes8 g_BattleEquipStateBlock __asm__("D_8009D1B0");
extern SaveBytes4 g_FieldMoveLock __asm__("D_8009D2E8");
extern SaveBytes4 g_SceneDispatchToken __asm__("D_8009D280");
extern SaveBytes4 g_GameStateFlags __asm__("D_8009D1A0");
extern SaveBytes4 D_800B0CDC;
extern s8 D_800B0CE0;
extern s8 g_LoadedTexturePageId __asm__("D_800B0CE1");
extern s8 g_SceneAreaType __asm__("D_800B0CE2");
extern s8 g_SavedSceneAreaType __asm__("D_800B0CE3");
extern s8 g_PendingStoryDay __asm__("D_800B0CE5");
extern s8 g_CurrentStoryDay __asm__("D_800B0CE4");
extern u8 g_DiscChangeFlags __asm__("D_800B0CE6");
extern u8 g_ScreenTransitionState __asm__("D_800BCFEE");
extern u8 *g_SaveIoCursor __asm__("D_800A0ED0");

void Save_SerializeTail(void) {
    register u8 *cursor asm("$3");
    register u8 *cd asm("$7");
    register u8 *t asm("$2");

    cd = g_SaveIoCursor;
    *(SaveBytes800 *)cd = g_EntityWorkBuffer;

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x800;
    *(SaveBytes4 *)(cursor + 0x800) = g_FieldMoveLock;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    *(SaveBytes4 *)(cursor + 4) = g_SceneDispatchToken;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    *(SaveBytes4 *)(cursor + 4) = g_GameStateFlags;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 4;
    *(SaveBytes4 *)(cursor + 4) = D_800B0CDC;
    {
        register int b0 asm("$2");
        register int b1 asm("$4");
        u8 u0;
        register u32 screenByte asm("$2");
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 4;
        asm volatile("" : : : "memory");
        b0 = D_800B0CE0;
        b1 = g_LoadedTexturePageId;
        asm volatile("" : : "r"(b0), "r"(b1));
        cursor[4] = b0;
        cursor[5] = b1;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 2;
        asm volatile("" : : : "memory");
        b0 = g_SceneAreaType;
        b1 = g_SavedSceneAreaType;
        asm volatile("" : : "r"(b0), "r"(b1));
        cursor[2] = b0;
        cursor[3] = b1;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 2;
        asm volatile("" : : : "memory");
        b0 = g_CurrentStoryDay;
        b1 = g_PendingStoryDay;
        asm volatile("" : : "r"(b0), "r"(b1));
        cursor[2] = b0;
        cursor[3] = b1;
        cursor = g_SaveIoCursor;
        u0 = g_DiscChangeFlags;
        g_SaveIoCursor = cursor + 2;
        cursor[2] = u0;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 1;
        asm volatile("" : : : "memory");
        screenByte = g_ScreenTransitionState;
        asm volatile("" : : "r"(screenByte));
        cursor[1] = screenByte;
    }
    t = g_SaveIoCursor + 1;
    cd = t;
    g_SaveIoCursor = cd;
    *(SaveBytes70 *)cd = g_AyaBattleState;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x70;
    *(SaveBytes18 *)(cursor + 0x70) = g_SavedBattleStateTail;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x18;
    *(SaveBytes8 *)(cursor + 0x18) = g_BattleEquipStateBlock;
    t = g_SaveIoCursor;
    g_SaveIoCursor = t + 8;
}
