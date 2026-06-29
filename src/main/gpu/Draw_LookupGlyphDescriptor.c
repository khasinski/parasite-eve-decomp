extern int D_800A8030;

void *Draw_LookupGlyphDescriptor(int index) {
    register char *base = (char *)&D_800A8030;
    register int offset = *(int *)base;

    base -= 8;
    return (void *)(offset + base + index * 8);
}
