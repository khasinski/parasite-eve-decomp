typedef unsigned char u8;

extern u8 D_80091A1C;

int Menu_IsEquipSlotActive(void) {
    return D_80091A1C != 0;
}
