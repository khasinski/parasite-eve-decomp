extern int D_800A803C[];

extern int D_800A8040;

extern int D_800A8048;

void *Aya_LookupLevelStats(int arg0) {
    if (arg0 < 0) {
        arg0 = 0;
    } else if (arg0 >= 0x63) {
        arg0 = 0x62;
    }

    return (void *)(D_800A803C[0] + (int)((char *)D_800A803C - 0x14) + (arg0 * 0x18));
}

void *Aya_GetLevelExpTable(void) {
    register char *base = (char *)&D_800A8040;
    register int offset = *(int *)base;

    base -= 0x18;
    return (void *)(offset + base);
}

void *Aya_GetParasiteSpellUnlockTable(void) {
    register char *base = (char *)&D_800A8048;
    register int offset = *(int *)base;

    base -= 0x20;
    return (void *)(offset + base);
}
