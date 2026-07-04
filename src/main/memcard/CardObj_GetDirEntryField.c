/* MASPSX_FLAGS: --stack-return-delay */
extern int (*g_MemCardObjLookupFn)(void);

int CardObj_GetDirEntryField(int arg0, int index0, int selector) {
    int obj;
    int entry;
    int sel;

    obj = g_MemCardObjLookupFn();
    if (index0 < 0) {
        return *(unsigned char *)(obj + 0xE9);
    }
    if (index0 >= *(unsigned char *)(obj + 0xE9)) {
        return 0;
    }
    entry = *(int *)(obj + 0x4) + ((index0 << 2) + index0);
    sel = selector - 1;
    if ((unsigned int)sel < 5) {
        switch (sel) {
        case 0:
            return *(unsigned char *)(entry + 0);
        case 1:
            return *(unsigned char *)(entry + 1);
        case 2:
            return *(unsigned char *)(entry + 2);
        case 3:
            return *(unsigned char *)(entry + 3);
        case 4:
            return *(unsigned char *)(entry + 4);
        }
    }
    return 0;
}
