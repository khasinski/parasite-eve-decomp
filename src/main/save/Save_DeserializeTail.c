#include "common.h"

#include "pe1/save_tail_state.h"

typedef struct SaveBytes12 { u8 b[12]; } SaveBytes12;

void Save_DeserializeTail(void) {
    register u8 *cursor asm("$3");
    register u8 *next_cur asm("$4");
    register SaveBytes70 *aya_dest asm("$8");
    register SaveBytes18 *battle_dest asm("$8");
    register SaveBytes8 *equip_dest asm("$8");
    register u8 *step18 asm("$2");
    register SaveBytes4 *field_dest asm("$7");
    register int b0 asm("$2");
    register int b1 asm("$5");
    register int b2 asm("$4");
    register u8 *third_cur asm("$7");
    register u8 *bulk_src asm("$6");
    register u8 *bulk_tmp asm("$2");
    register u32 game_mask asm("$5");
    u8 screen_byte;
    register int field_mask asm("$4");
    u32 game_word;
    u32 field_word;

    cursor = g_SaveIoCursor;
    g_EntityWorkBuffer = *(SaveBytes800 *)cursor;
    cursor = g_SaveIoCursor;
    asm volatile("" : : "r"(cursor));
    aya_dest = &g_AyaBattleState;
    asm volatile("" : : "r"(aya_dest));
    g_SaveIoCursor = cursor + 0x800;
    asm volatile("" ::: "memory");
    next_cur = g_SaveIoCursorRead;
    asm volatile("" : : "r"(next_cur));
    field_dest = &g_FieldMoveLock;
    asm volatile("" : : "r"(field_dest));
    *field_dest = *(SaveBytes4 *)(cursor + 0x800);
    g_SaveIoCursor = next_cur + 4;
    asm volatile("" ::: "memory");
    cursor = g_SaveIoCursorRead;
    asm volatile("" : : "r"(cursor));
    field_dest = &g_SceneDispatchToken;
    asm volatile("" : : "r"(field_dest));
    *field_dest = *(SaveBytes4 *)(next_cur + 4);

    g_SaveIoCursor = cursor + 4;
    asm volatile("" ::: "memory");
    next_cur = g_SaveIoCursorRead;
    asm volatile("" : : "r"(next_cur));
    field_dest = &g_GameStateFlags;
    asm volatile("" : : "r"(field_dest));
    *field_dest = *(SaveBytes4 *)(cursor + 4);

    g_SaveIoCursor = next_cur + 4;
    asm volatile("" ::: "memory");
    cursor = g_SaveIoCursorRead;
    asm volatile("" : : "r"(cursor));
    field_dest = &D_800B0CDC;
    asm volatile("" : : "r"(field_dest));
    *field_dest = *(SaveBytes4 *)(next_cur + 4);

    g_SaveIoCursor = cursor + 4;
    asm volatile("" ::: "memory");
    next_cur = g_SaveIoCursorRead;
    asm volatile("" : : "r"(next_cur));
    b0 = ((s8 *)cursor)[4];
    b1 = ((s8 *)cursor)[5];
    asm volatile("" : : "r"(b0), "r"(b1));
    D_800B0CE0 = b0;
    g_LoadedTexturePageId = b1;
    asm volatile("" ::: "memory");

    g_SaveIoCursor = next_cur + 2;
    asm volatile("" ::: "memory");
    cursor = g_SaveIoCursorRead;
    asm volatile("" : : "r"(cursor));
    b0 = ((s8 *)next_cur)[2];
    b1 = ((s8 *)next_cur)[3];
    asm volatile("" : : "r"(b0), "r"(b1));
    g_SceneAreaType = b0;
    g_SavedSceneAreaType = b1;
    asm volatile("" ::: "memory");

    g_SaveIoCursor = cursor + 2;
    asm volatile("" ::: "memory");
    third_cur = g_SaveIoCursorRead;
    asm volatile("" : : "r"(third_cur));
    b0 = ((s8 *)cursor)[2];
    b2 = ((s8 *)cursor)[3];
    asm volatile("" : : "r"(b0), "r"(b2));
    g_CurrentStoryDay = b0;
    g_PendingStoryDay = b2;
    asm volatile("" ::: "memory");

    bulk_tmp = third_cur + 4;
    asm volatile("" : : "r"(bulk_tmp));
    bulk_src = bulk_tmp;
    asm volatile("" : : "r"(bulk_src));
    asm volatile("" : : "r"((u32)bulk_src | (u32)aya_dest));
    g_SaveIoCursor = third_cur + 2;
    g_DiscChangeFlags = third_cur[2];
    g_SaveIoCursor = third_cur + 3;
    screen_byte = third_cur[3];
    asm volatile("" : : "r"(screen_byte));
    asm volatile("" : : "r"(((u32)bulk_src | (u32)aya_dest) & 3));
    g_ScreenTransitionState = screen_byte;
    asm volatile("" ::: "memory");
    g_SaveIoCursor = bulk_src;

    cursor = g_SaveIoCursor;
    *aya_dest = *(SaveBytes70 *)bulk_src;

    cursor = g_SaveIoCursor;
    g_SaveIoCursor = cursor + 0x70;
    game_mask = 0xFFFF2679;
    asm volatile("" ::: "memory");
    next_cur = g_SaveIoCursorRead;
    asm volatile("" : : "r"(next_cur));
    battle_dest = &g_SavedBattleStateTail;
    asm volatile("" : : "r"(battle_dest));
    *(SaveBytes12 *)battle_dest = *(SaveBytes12 *)(cursor + 0x70);
    asm volatile("" : "=r"(battle_dest) : "0"(battle_dest));
    *(SaveBytes12 *)((u8 *)battle_dest + 12) = *(SaveBytes12 *)(cursor + 0x7C);

    step18 = next_cur + 0x18;
    asm volatile("" : : "r"(step18));
    g_SaveIoCursor = step18;
    asm volatile("" ::: "memory");
    equip_dest = &g_BattleEquipStateBlock;
    asm volatile("" : : "r"(equip_dest));
    *equip_dest = *(SaveBytes8 *)(next_cur + 0x18);
    asm volatile("" : : "r"(step18));

    field_mask = -10;
    game_word = g_GameStateFlagsWord;
    cursor = g_SaveIoCursor;
    g_GameStateFlagsWord = game_word & game_mask;
    field_word = g_FieldMoveLockWord;
    g_SaveIoCursor = cursor + 8;
    g_FieldMoveLockWord = field_word & field_mask;
}
