extern int D_800A8040;

void *Aya_GetLevelExpTable(void) {
    register char *base = (char *)&D_800A8040;
    register int offset = *(int *)base;

    base -= 0x18;
    return (void *)(offset + base);
}
