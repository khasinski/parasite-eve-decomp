/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

typedef unsigned char u8;

int g_MenuItemDiscardMode;
u8 *g_MenuSelectedItemData;

void Inv_SetActiveList(int arg0, void *arg1);
void Menu_PlayConfirmSound(void);

unsigned char *g_MenuSelectedItemData;

void Draw_OffsetCursor(int x, int y);
void *Str_LookupTableC(unsigned int arg0);
void Draw_PrintRawText(u8 *arg0);

int Menu_HandleItemInput(int arg0, int arg1) {
    int value;
    u8 *ptr;

    if ((arg1 & 0x10040) != 0) {
        if (g_MenuItemDiscardMode != 0) {
            ptr = g_MenuSelectedItemData;
            g_MenuItemDiscardMode = 0;
            value = ptr[4];
            Inv_SetActiveList(0, &value);
        } else {
            Inv_SetActiveList(9, 0);
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_DrawEquipItemName(void) {
    Draw_OffsetCursor(4, 4);
    Draw_PrintRawText(Str_LookupTableC(g_MenuSelectedItemData[4] - 1));
}
