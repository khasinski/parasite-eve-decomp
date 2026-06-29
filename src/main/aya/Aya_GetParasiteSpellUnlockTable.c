extern int D_800A8048;

void *Aya_GetParasiteSpellUnlockTable(void) {
    register char *base = (char *)&D_800A8048;
    register int offset = *(int *)base;

    base -= 0x20;
    return (void *)(offset + base);
}
