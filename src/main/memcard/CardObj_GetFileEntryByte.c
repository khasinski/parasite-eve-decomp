/* MASPSX_FLAGS: --stack-return-delay */

extern int (*g_MemCardObjLookupFn)(void);

int CardObj_GetFileEntryByte(int arg0, int index0, int index1) {
    int obj;
    int entry;

    obj = g_MemCardObjLookupFn();
    if (index0 < 0) {
        return *(unsigned char *)(obj + 0xEA);
    }
    if (index0 >= *(unsigned char *)(obj + 0xEA)) {
        return 0;
    }
    entry = *(int *)(obj + 0x8) + (index0 << 3);
    if (index1 < 0) {
        return *(unsigned char *)entry;
    }
    if (index1 >= *(unsigned char *)entry) {
        return 0;
    }
    return *(unsigned char *)(*(int *)(entry + 4) + index1);
}
