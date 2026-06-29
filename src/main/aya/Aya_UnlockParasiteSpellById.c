typedef unsigned short u16;

extern unsigned int g_AyaParasiteSpellFlags;

void *Aya_GetParasiteSpellUnlockTable(void);
void Menu_CreateParasiteSpellInfoPanel(int arg0);

int Aya_UnlockParasiteSpellById(int arg0) {
    u16 *entry;
    int i;
    register int found asm("$16");
    unsigned int *flags;
    register int bit asm("$3");
    u16 *start;

    start = Aya_GetParasiteSpellUnlockTable();
    i = 0;
    entry = start;
loop:
    if (*entry == arg0) {
        goto done;
    }
    i++;
    if (i < 20) {
        entry += 2;
        goto loop;
    }

done:
    found = i < 20;
    if (found) {
        bit = 1;
        flags = &g_AyaParasiteSpellFlags;
        *flags |= bit << i;
        Menu_CreateParasiteSpellInfoPanel(i);
    }
    return found;
}
