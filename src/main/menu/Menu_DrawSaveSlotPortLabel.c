typedef struct {
    char pad[0x24];
    int value;
} Inner;

typedef struct {
    char pad[4];
    Inner *inner;
} Outer;

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_FlushPrimList(int arg0);

void Menu_DrawSaveSlotPortLabel(Outer *arg0) {
    Draw_OffsetCursor(0, 2);
    Draw_FlushPrimList(arg0->inner->value - 0x11);
}
