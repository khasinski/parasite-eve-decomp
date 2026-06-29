/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

int g_MenuItemDiscardMode;
u8 *g_MenuSelectedItemData;

void Inv_SetActiveList(int arg0, void *arg1);
void Menu_PlayConfirmSound(void);

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
