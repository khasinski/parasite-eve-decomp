typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;

extern u8 g_TextboxEntries[];
extern s16 D_800BCEB8[];

s8 Tbl_LookupEntry(int arg0) {
    int i;
    int idx;
    int offset;
    int value;

    value = 0;
    i = 0;
    arg0 = (s16)arg0;
    while ((unsigned char)i < 4) {
        idx = (unsigned char)i;
        offset = ((idx << 3) - idx) << 3;
        if (*(s16 *)((u8 *)D_800BCEB8 + offset) == arg0) {
            value = *(u8 *)((u8 *)g_TextboxEntries + offset);
            break;
        }
        i++;
    }
    return value;
}
