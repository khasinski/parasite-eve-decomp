typedef unsigned int u32;

extern int g_FieldRenderFlagTable[];

static int Gte_HighestBitIndex(u32 mask) {
    int index;

    index = 31;
    while (index > 0 && (mask & 0x80000000) == 0) {
        mask <<= 1;
        index--;
    }
    return index;
}

void Gte_StoreTableEntry(u32 mask, int value) {
    g_FieldRenderFlagTable[Gte_HighestBitIndex(mask)] = value;
}
