
extern char func_800119CC[];
extern char *g_CdIntrStringTable[];

char *CdIntstr(unsigned int index) {
    index &= 0xFF;
    if (index >= 7) {
        return func_800119CC;
    }
    return g_CdIntrStringTable[index];
}
