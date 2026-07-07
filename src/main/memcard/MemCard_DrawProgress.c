/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MemCardProgressPrimList;

void Draw_OffsetCursor(int x, int y);
void Draw_FlushPrimList(int arg0);
int MemCard_GetActiveProgressBlocks(void);
void Draw_AllocSprite(int arg0);

void MemCard_DrawProgress(void) {
    int i;
    int count;

    Draw_OffsetCursor(0, 10);
    Draw_FlushPrimList(g_MemCardProgressPrimList);
    if (g_MemCardProgressPrimList != 0x40) {
        count = MemCard_GetActiveProgressBlocks();
        i = 0;
        Draw_OffsetCursor(0x10, 0x14);
        while (i < 8) {
            if (i < count) {
                Draw_AllocSprite(0x83);
            } else {
                Draw_AllocSprite(0x82);
            }
            i++;
            Draw_OffsetCursor(0x10, 0);
        }
    }
}
