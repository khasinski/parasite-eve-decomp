typedef unsigned char u8;

extern u8 D_80091A20;

int Menu_ConsumeEquipSlotFlag(void) {
    u8 *ptr;

    ptr = &D_80091A20;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}
