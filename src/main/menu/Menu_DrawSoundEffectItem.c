/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSelectedItemData;

void Sfx_CursorRenderData(int);

void Menu_DrawSoundEffectItem(void) {
    Sfx_CursorRenderData(g_MenuSelectedItemData);
}
