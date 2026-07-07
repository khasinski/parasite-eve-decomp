/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern unsigned int g_AyaParasiteSpellFlags[];
extern short g_InvPackedList[];
int g_InvPackedListCursor;
int g_InvPackedListCount;

void Item_PackParasiteSpells(void) {
    int flags;
    short *out;
    int i;

    flags = g_AyaParasiteSpellFlags[0];
    out = g_InvPackedList;

    for (i = 0; i < 0x14; i++) {
        if (flags & 1) {
            *out++ = i;
        }
        flags >>= 1;
    }

    g_InvPackedListCursor = 0;
    g_InvPackedListCount = out - g_InvPackedList;
}
