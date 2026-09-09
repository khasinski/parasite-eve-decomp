#include "common.h"
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
#include "common.h"
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
#include "common.h"
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
