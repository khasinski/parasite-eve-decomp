extern int D_800A8044;

void *Str_ResolveDataPtr(void) {
    register char *base = (char *)&D_800A8044;
    register int offset = *(int *)base;

    base -= 0x1C;
    return (void *)(offset + base);
}
