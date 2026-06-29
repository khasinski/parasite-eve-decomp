
extern unsigned char g_MemCardObjStorage[];

unsigned char *CardObj_LookupByChannelId(int value) {
    unsigned char *entry = g_MemCardObjStorage;

    if ((value & 0xF0) != 0) {
        entry += 0xF0;
    }
    return entry;
}
