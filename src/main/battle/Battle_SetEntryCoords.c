
typedef struct {
    void *entries;
} struct_8002FAD8;

void Battle_SetEntryCoords(struct_8002FAD8 *arg0, unsigned char arg1, int arg2, int arg3) {
    int offset = (arg1 & 0xFF) << 4;
    char *base = arg0->entries;

    offset += 0x1C;
    base += offset;
    *(int *)(base + 4) = arg2;
    *(int *)(base + 8) = arg3;
}
