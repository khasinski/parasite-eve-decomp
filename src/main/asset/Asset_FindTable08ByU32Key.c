typedef unsigned int u32;
typedef unsigned char u8;
typedef int s32;

int Asset_FindTable08ByU32Key(void *arg0, s32 arg1) {
    u32 mask;
    register u8 *table asm("$2");
    u32 descriptor;
    register u8 *record asm("$6");
    register s32 none asm("$7");
    register u32 i asm("$8");
    u32 data_mask;
    char frame[1];

    asm volatile("" : : "r"(frame));

    mask = 0x3FFFFF;
    descriptor = *(u32 *)((u8 *)arg0 + *(u32 *)((u8 *)arg0 + 4) + 8);
    i = 0;
    none = 0;
    table = (u8 *)arg0 + (descriptor & mask);
    descriptor >>= 22;

    if (descriptor != 0) {
        data_mask = 0xFFFFFF;
        record = table;
        do {
            if (*(s32 *)(record + 8) == arg1) {
                return (s32)((u8 *)arg0 + (*(u32 *)(record + 4) & data_mask));
            }
            i++;
            if (i < descriptor) {
                record += 0xC;
                continue;
            }
            break;
        } while (1);
    }

    return none;
}
