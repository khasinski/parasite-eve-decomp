extern unsigned char g_AyaInventorySlotCount[];

int Inv_GetBonusSlotCount(void);

int Inv_GetAyaSlotLimit(void) {
    if (g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount() >= 0x33) {
        return 0x32;
    }

    return g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount();
}
