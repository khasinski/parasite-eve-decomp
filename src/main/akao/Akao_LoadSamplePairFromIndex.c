/* CC1_PROFILE: build0001 */
typedef unsigned short u16;
typedef unsigned int u32;

extern u16 *D_8009D240;
extern char *D_8009D260;

void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2) {
    u32 index;
    u16 *table;
    register char *addr asm("$3");
    u16 value;
    int result;

    index = arg2;
    index &= 0x3FF;
    index <<= 1;
    table = D_8009D240;
    addr = (char *)(index << 1);
    addr += (int)table;
    value = *(u16 *)addr;
    if (value != 0xFFFF) {
        result = (int)(D_8009D260 + value);
    } else {
        result = 0;
    }
    *arg0 = result;

    index++;
    table = D_8009D240;
    addr = (char *)(index << 1);
    addr += (int)table;
    value = *(u16 *)addr;
    if (value != 0xFFFF) {
        result = (int)(D_8009D260 + value);
    } else {
        result = 0;
    }
    *arg1 = result;
}
