typedef unsigned char u8;
typedef unsigned int u32;

typedef struct AssetSlot {
    u8 pad[0x18];
    void (*unload)(void);
} AssetSlot;

extern u32 D_8009D1A0;
extern u8 D_800B0CD8[];
extern AssetSlot **D_800942E0;
extern void *D_800E10BC[];

int Asset_UnloadTableEntries(void) {
    int i;
    AssetSlot *slot;
    u8 *base;
    u8 *record;
    u8 *entry;
    u32 packed;
    int count;

    if (D_8009D1A0 & 0x80) {
        for (i = 0; i < 8; i++) {
            slot = D_800942E0[i];
            if (slot != 0 && slot->unload != 0) {
                slot->unload();
            }
        }

        slot = *(AssetSlot **)((u8 *)D_800942E0 + 0x154);
        if (slot != 0 && slot->unload != 0) {
            slot->unload();
        }

        base = *(u8 **)(D_800B0CD8 + 0x18C);
        record = base + *(u32 *)(base + 4);
        packed = *(u32 *)(record + 4);
        count = packed >> 22;
        entry = base + (packed & 0x3FFFFF);

        for (i = 0; i < count; i++, entry += 0xC) {
            u8 id = entry[7];
            if ((u32)(id - 8) < 0x4D) {
                slot = D_800942E0[id];
                if (slot != 0 && slot->unload != 0) {
                    slot->unload();
                }
            }
        }

        D_8009D1A0 &= ~0x80;
    }

    for (i = 8; i < 0x55; i++) {
        if (D_800942E0[i] != 0) {
            D_800942E0[i] = 0;
        }
    }

    for (i = 0x1E; i < 0x68; i++) {
        if (D_800E10BC[i - 0x1E] != 0) {
            D_800E10BC[i - 0x1E] = 0;
        }
    }

    return 0;
}
