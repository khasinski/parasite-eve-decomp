/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSpendArrowDirection;

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_AllocSprite(int arg0);
void Menu_DrawWeaponComparisonPanel(void);

void Menu_DrawAmmoSpendPanel(void) {
    Draw_OffsetCursor(0x3C, 0x12);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0x38, -0xE);
    Menu_DrawWeaponComparisonPanel();
}
