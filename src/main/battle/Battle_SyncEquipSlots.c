extern unsigned int *g_PlayerEntity;
void Inv_RecalcSlotStats(void);

void Battle_SyncEquipSlots(void) {
    unsigned int *outer;
    unsigned short *ptr;
    unsigned short value;

    Inv_RecalcSlotStats();
    outer = g_PlayerEntity;
    if (outer != 0) {
        ptr = (unsigned short *)outer[0];
        if (ptr != 0) {
            value = ptr[0xE];
            ptr[7] = value;
            ptr[6] = value;
        }
    }
}
