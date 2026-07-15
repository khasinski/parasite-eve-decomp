/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CEF4;

void func_80050618(void);
void MenuWidget_DrawList(void *node, void (*draw)(void));
void Draw_SetTextDimmed(int dimmed);
void Draw_AllocSprite(int sprite);
void Draw_OffsetCursor(int x, int y);

void Menu_DrawEquipListContainer(void *node) {
    int count;

    D_8009CEF4 = (int)node;
    MenuWidget_DrawList(node, func_80050618);
    Draw_SetTextDimmed(1);

    count = *(int *)((char *)node + 0x38);
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}
