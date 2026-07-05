/* MASPSX_FLAGS: --stack-return-delay */

extern int (*g_MemCardObjLookupFn)(void);

void CardObj_BeginCommand4D(int arg0, int arg1);

void CardObj_BeginReadIdCommand(int arg0, int arg1, int arg2);

void CardObj_SetPayload4D(int arg0, int arg1, int arg2);

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

void CardObj_StartCommand4D(int arg0, int arg1) {
    CardObj_BeginCommand4D(g_MemCardObjLookupFn(), arg1);
}

void CardObj_StartCommandWithBytes(int arg0, unsigned char arg1, unsigned char arg2) {
    CardObj_BeginReadIdCommand(g_MemCardObjLookupFn(), arg1, arg2);
}

void CardObj_SetCommandPayload(int arg0, int arg1, int arg2) {
    CardObj_SetPayload4D(g_MemCardObjLookupFn(), arg1, arg2);
}
