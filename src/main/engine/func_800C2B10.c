extern char *D_800E2248;

void *func_800C2B10(int index) {
    int offset;
    char *base;

    offset = index << 2;
    base = D_800E2248;
    offset += 8;
    return base + offset;
}
