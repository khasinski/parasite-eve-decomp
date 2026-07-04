/* MASPSX_FLAGS: -G8 */
/* CC1_FLAGS: -G8 */

extern unsigned char *g_MenuActionItemData;
extern int g_InvItemUsableFlag;

void Draw_AllocSprite(int);

void Menu_DrawActionCodeItem(int arg0) {
    int code;

    if (arg0 < g_MenuActionItemData[0x14]) {
        code = (&g_MenuActionItemData[arg0])[0x15] & 0x1F;
        if (code == 0) {
            return;
        }

        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(code + 0x22);
        } else {
            Draw_AllocSprite(code + 0x36);
        }
    } else {
        Draw_AllocSprite(0x46);
    }
}
