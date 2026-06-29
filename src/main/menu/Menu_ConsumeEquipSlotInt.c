extern int D_80091A24;

int Menu_ConsumeEquipSlotInt(void) {
    int *ptr;

    ptr = &D_80091A24;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}
