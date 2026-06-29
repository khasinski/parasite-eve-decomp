extern int g_MemCardCommandByte;
extern void *g_MemCardObjArray;

int MemCard_IsPortReady(int arg0) {
    register int index asm("$3");
    int offset;
    int addr;

    index = arg0 >> 4;
    if (g_MemCardCommandByte != 0) {
        offset = ((index << 4) - index) << 4;
        addr = offset + (int)g_MemCardObjArray;
        return *(unsigned char *)(addr + 0xE8) == 8;
    }
    return 0;
}
