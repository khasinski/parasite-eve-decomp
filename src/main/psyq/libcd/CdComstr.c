/* MASPSX_FLAGS: -G8 */

extern char func_800119CC[];
extern char *g_CdCmdNameTable[];

char *CdComstr(unsigned int index) {
    index &= 0xFF;
    if (index >= 0x1C) {
        return func_800119CC;
    }
    return g_CdCmdNameTable[index];
}
