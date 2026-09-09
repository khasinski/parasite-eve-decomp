/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;

void Menu_DrawItemSlot(int arg0);
void MenuWidget_DrawList();
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

void Menu_DrawSelectableEquipSlotRow(int arg0);
void Inv_SelectActiveList(int useOverride);

void Menu_DrawSelectableEquipSlotList(int arg0) {
    Inv_SelectActiveList(0);
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawSelectableEquipSlotRow);
}

int g_MenuActiveItemList;

void Sfx_ContextPick(int arg0);

void Menu_DrawContextActionList(int arg0) {
    int saved = arg0;

    Inv_SelectActiveList(g_MenuActiveItemList);
    MenuWidget_DrawList(saved, Sfx_ContextPick);
}
