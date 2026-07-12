extern char *D_800E2248;

int FieldEng_Spawn6(char *base, int unused, int index, int value) {
    base += 0xC;
    D_800E2248 = base;
    *(int *)(base + index * 4 + 0x48) = value;
    return 0;
}
