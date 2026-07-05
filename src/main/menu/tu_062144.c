/* CC1_PROFILE: build0001 */

int Str_GetTableEntryA(int arg0) {
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        return arg0 + 0xC;
    }
    return 0;
}

void *Widget_GetDataPtr(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return base + 4;
}

int Str_GetTableEntryB(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return (int)(base + 0xC);
}

int Str_GetTableEntryC(int arg0) {
    if ((*(int *)(arg0 + 4) & 8) != 0) {
        return arg0 + 0x14;
    }
    return 0;
}
