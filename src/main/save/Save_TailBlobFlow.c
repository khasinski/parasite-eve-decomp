#include "common.h"

#include "pe1/save_tail_state.h"

void Save_SerializeTail(void) {
    register u8 *cursor asm("$3");
    u8 *cd;

    {
        u8 *entityDestination = g_SaveIoCursor;
        *(SaveBytes800 *)entityDestination = g_EntityWorkBuffer;
    }

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
        int b0;
        int b1;
        u8 u0;
        u32 screenByte;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 4;
        asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
        b0 = D_800B0CE0;
        b1 = g_LoadedTexturePageId;
        cursor[4] = b0;
        cursor[5] = b1;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 2;
        asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
        b0 = g_SceneAreaType;
        b1 = g_SavedSceneAreaType;
        cursor[2] = b0;
        cursor[3] = b1;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 2;
        asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
        b0 = g_CurrentStoryDay;
        b1 = g_PendingStoryDay;
        cursor[2] = b0;
        cursor[3] = b1;
        cursor = g_SaveIoCursor;
        u0 = g_DiscChangeFlags;
        g_SaveIoCursor = cursor + 2;
        cursor[2] = u0;
        cursor = g_SaveIoCursor;
        g_SaveIoCursor = cursor + 1;
        screenByte = g_ScreenTransitionState;
        asm volatile("" : : "r"(screenByte));
        cursor[1] = screenByte;
    }
    cd = g_SaveIoCursor + 1;
    g_SaveIoCursor = cd;
    *(SaveBytes70 *)cd = g_AyaBattleState;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x70;
    *(SaveBytes18 *)(cursor + 0x70) = g_SavedBattleStateTail;
    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x18;
    *(SaveBytes8 *)(cursor + 0x18) = g_BattleEquipStateBlock;
    {
        u8 *end = g_SaveIoCursor;
        g_SaveIoCursor = end + 8;
    }
}

void Save_DeserializeTail(void) {
    register u8 *cursor asm("$3");
    register u8 *next_cur asm("$4");
    SaveBytes70 *aya_dest;
    SaveBytes18 *battle_dest;
    SaveBytes8 *equip_dest;
    u8 *step18;
    SaveBytes4 *field_dest;
    int b0;
    register int b1 asm("$5");
    int restoreValue;
    register u8 *third_cur asm("$7");
    register u8 *bulk_src asm("$6");
    register u8 *bulk_tmp asm("$2");
    u32 game_mask;
    u8 screen_byte;
    u32 game_word;
    u32 field_word;

    {
        u8 *entitySource = g_SaveIoCursor;
        g_EntityWorkBuffer = *(SaveBytes800 *)entitySource;
    }
    cursor = g_SaveIoCursor;
    asm volatile("" : : "r"(cursor));
    aya_dest = &g_AyaBattleState;
    asm volatile("" : : "r"(aya_dest));
    g_SaveIoCursor = cursor + 0x800;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    next_cur = g_SaveIoCursorRead;
    field_dest = &g_FieldMoveLock;
    *field_dest = *(SaveBytes4 *)(cursor + 0x800);
    g_SaveIoCursor = next_cur + 4;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    cursor = g_SaveIoCursorRead;
    field_dest = &g_SceneDispatchToken;
    *field_dest = *(SaveBytes4 *)(next_cur + 4);

    g_SaveIoCursor = cursor + 4;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    next_cur = g_SaveIoCursorRead;
    field_dest = &g_GameStateFlags;
    *field_dest = *(SaveBytes4 *)(cursor + 4);

    g_SaveIoCursor = next_cur + 4;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    cursor = g_SaveIoCursorRead;
    field_dest = &D_800B0CDC;
    *field_dest = *(SaveBytes4 *)(next_cur + 4);

    g_SaveIoCursor = cursor + 4;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    next_cur = g_SaveIoCursorRead;
    b0 = ((s8 *)cursor)[4];
    b1 = ((s8 *)cursor)[5];
    asm volatile("" : : "r"(b0), "r"(b1));
    D_800B0CE0 = b0;
    g_LoadedTexturePageId = b1;

    g_SaveIoCursor = next_cur + 2;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    cursor = g_SaveIoCursorRead;
    b0 = ((s8 *)next_cur)[2];
    b1 = ((s8 *)next_cur)[3];
    asm volatile("" : : "r"(b0), "r"(b1));
    g_SceneAreaType = b0;
    g_SavedSceneAreaType = b1;

    g_SaveIoCursor = cursor + 2;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    third_cur = g_SaveIoCursorRead;
    asm volatile("" : : "r"(third_cur));
    b0 = ((s8 *)cursor)[2];
    restoreValue = ((s8 *)cursor)[3];
    g_CurrentStoryDay = b0;
    g_PendingStoryDay = restoreValue;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));

    bulk_tmp = third_cur + 4;
    bulk_src = bulk_tmp;
    asm volatile("" : : "r"(bulk_src),
        "r"((u32)bulk_src | (u32)aya_dest));
    g_SaveIoCursor = third_cur + 2;
    g_DiscChangeFlags = third_cur[2];
    g_SaveIoCursor = third_cur + 3;
    screen_byte = third_cur[3];
    asm volatile("" : : "r"(screen_byte));
    g_ScreenTransitionState = screen_byte;
    g_SaveIoCursor = bulk_src;

    *aya_dest = *(SaveBytes70 *)bulk_src;

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x70;
    game_mask = 0xFFFF2679;
    asm volatile("" : "=m"(g_SaveIoCursorRead) : "m"(g_SaveIoCursorRead));
    next_cur = g_SaveIoCursorRead;
    battle_dest = &g_SavedBattleStateTail;
    *battle_dest = *(SaveBytes18 *)(cursor + 0x70);

    step18 = next_cur + 0x18;
    g_SaveIoCursor = step18;
    equip_dest = &g_BattleEquipStateBlock;
    *equip_dest = *(SaveBytes8 *)(next_cur + 0x18);
    asm volatile("" : : "r"(step18));

    restoreValue = -10;
    game_word = g_GameStateFlagsWord;
    cursor = g_SaveIoCursor;
    g_GameStateFlagsWord = game_word & game_mask;
    field_word = g_FieldMoveLockWord;
    g_SaveIoCursor = cursor + 8;
    g_FieldMoveLockWord = field_word & restoreValue;
}
