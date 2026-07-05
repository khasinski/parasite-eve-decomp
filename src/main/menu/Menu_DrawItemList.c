/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;

void Menu_DrawItemSlot(int arg0);
void MenuWidget_DrawList(void *arg0, void (*arg1)(int));
void Draw_SetTextDimmed(int value);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int x, int y);
int Battle_GetStateFlag1(void);
void Battle_DrawActiveStatus(void);

void Menu_DrawItemList(void *arg0) {
    int count;

    g_MenuActiveWidget = (int)arg0;
    MenuWidget_DrawList(arg0, Menu_DrawItemSlot);
    Draw_SetTextDimmed(1);

    count = *(int *)((char *)arg0 + 0x38);
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }

    if (Battle_GetStateFlag1() == 0) {
        if (g_MenuBattleStatusOverlayActive != 0) {
            Battle_DrawActiveStatus();
        }
    }
}
