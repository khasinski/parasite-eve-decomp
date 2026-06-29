typedef unsigned char u8;
typedef unsigned int u32;

extern u8 g_TextboxEntries[];
extern u32 D_800BCEB4[];

void Tbl_ResetAll(void) {
    int i;
    int idx;
    int offset;
    u32 value;

    for (i = 0; (unsigned char)i < 4; i++) {
        idx = (unsigned char)i;
        offset = ((idx << 3) - idx) << 3;
        value = *(u32 *)((u8 *)D_800BCEB4 + offset);
        *(u8 *)((u8 *)g_TextboxEntries + offset) = 0;
        value &= 0xFDFFFFFF;
        *(u32 *)((u8 *)D_800BCEB4 + offset) = value;
    }
}
