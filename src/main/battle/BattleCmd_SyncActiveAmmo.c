
/* Incomplete arrays: retail addresses these absolutely (g_PlayerEntity is
 * in the gp window but not small data; the Aya fields are outside it). */
extern void **g_PlayerEntity[];
extern short g_AyaHpCurrent[];
extern signed char g_AyaEquippedWeaponSlot[];

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int useOverride);
void *Inv_LookupActiveListData(int index);

void BattleCmd_SyncActiveAmmo(void) {
    void **entity;
    void *current;
    void *entry;
    int saved;

    entity = g_PlayerEntity[0];
    if (entity != 0) {
        current = entity[0];
        if (current != 0) {
            g_AyaHpCurrent[0] = *(unsigned short *)((char *)current + 0xC);
            if (*(void **)((char *)current + 0x68) != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    *(short *)((char *)entry + 0xA) =
                        *(int *)((char *)*(void **)((char *)current + 0x68) + 0xC) & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}
