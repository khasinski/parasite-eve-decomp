/* MASPSX_FLAGS: --stack-return-delay */

extern int (*g_MemCardObjLookupFn)(void);

int CardObj_GetField(int arg0, int mode, int index) {
    int obj;

    obj = g_MemCardObjLookupFn();
    switch (mode) {
    case 1:
        return *(unsigned char *)(obj + 0xE8);
    case 2:
        return *(unsigned short *)(obj + 0xE6);
    case 3:
        return *(unsigned char *)(obj + 0xE4);
    case 4:
        if (index < 0) {
            return *(unsigned char *)(obj + 0xE3);
        }
        if (index < *(unsigned char *)(obj + 0xE3)) {
            return *(unsigned short *)((index << 1) + *(int *)obj);
        }
        goto late_fail;
    case 100:
        return *(int *)(obj + 0x4C);
    default:
        return 0;
    }

late_fail:
    return 0;
}
