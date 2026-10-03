#include "common.h"

extern u8 g_MapNameOverrideA[];
extern u8 g_MapNameOverrideB[];
extern u8 g_MapSelectIndexTable[];
extern u8 g_MapFilenameTable[];
extern int g_SceneDispatchToken;
extern int g_SceneDispatchCur;
extern u32 g_GameStateFlags;
extern u32 g_GameState[];

int Menu_IsEquipSlotActive();
int Render_LoadFontGlyphAlt();
int Menu_GetEquipSlotStateOrIndex(void);
int Render_SetFontGlyphByCode();
void Menu_ResetEquipSlotState(void);
int Str_ParseBase32Id(u8 *text);

int Task_SetFloorByEntityRoll(u8 **args) {
    int selection;
    u8 state;
    u8 *select_index;
    u32 *game_state;
    u32 map_flags;
    u32 state_flags;

    if (!(Menu_IsEquipSlotActive() & 0xFF)) {
        Render_LoadFontGlyphAlt();
    }
    selection = Render_SetFontGlyphByCode(**args) & 0xFF;

    if (selection == 0xFF) {
        if ((u8)Menu_GetEquipSlotStateOrIndex() < 2) {
            Menu_ResetEquipSlotState();
            g_SceneDispatchToken = Str_ParseBase32Id(g_MapNameOverrideA);
        } else {
            Menu_ResetEquipSlotState();
            g_SceneDispatchToken = Str_ParseBase32Id(g_MapNameOverrideB);
        }
        return 1;
    }

    state = (u8)(Menu_GetEquipSlotStateOrIndex() - 1);
    state /= 10;
    select_index = g_MapSelectIndexTable + state * 24;
    g_SceneDispatchToken =
        Str_ParseBase32Id(g_MapFilenameTable + ((int)select_index[selection] << 3));

    selection = 1;
    if (g_SceneDispatchCur == g_SceneDispatchToken) {
        game_state = g_GameState;
        state_flags = g_GameStateFlags;
        map_flags = game_state[0];
        g_GameStateFlags = state_flags | 0x2000;
        game_state[0] = map_flags | 0x800;
    } else {
        return selection;
    }
    return selection;
}
