int Inv_GetPackedListCount(void);
int Inv_GetPackedListItem(int);
int Str_LookupTable8(int);
void Draw_PrintRawText(int);

void Menu_DrawModNameItem(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        Draw_PrintRawText(Str_LookupTable8(Inv_GetPackedListItem(arg0) + 0xEB));
    }
}
