/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveItemList;

void Sfx_ContextPick(int arg0);
void Inv_SelectActiveList(int useOverride);
void MenuWidget_DrawList(int arg0, void (*callback)(int));

void Menu_DrawContextActionList(int arg0) {
    int saved = arg0;

    Inv_SelectActiveList(g_MenuActiveItemList);
    MenuWidget_DrawList(saved, Sfx_ContextPick);
}
