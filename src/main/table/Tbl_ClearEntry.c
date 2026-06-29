typedef unsigned char u8;
typedef signed short s16;

extern u8 g_TextboxEntries[];
extern s16 D_800BCEB8[];

void Tbl_ClearEntry(int arg0) {
    int i;
    int idx;
    int offset;

    i = 0;
    arg0 = (s16)arg0;
    while ((unsigned char)i < 4) {
        idx = (unsigned char)i;
        offset = ((idx << 3) - idx) << 3;
        if (*(s16 *)((u8 *)D_800BCEB8 + offset) == arg0) {
            if (*(u8 *)((u8 *)g_TextboxEntries + offset) != 0) {
                *(u8 *)((u8 *)g_TextboxEntries + offset) = 0;
                break;
            }
        }
        i++;
    }
}
