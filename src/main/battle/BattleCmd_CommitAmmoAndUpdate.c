
extern void **g_PlayerEntity;
extern short g_AyaHpCurrent;
extern signed char g_AyaEquippedWeaponSlot;

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int);
void *Inv_LookupActiveListData(int);
int Inv_DrawSlotItemIcon(void);
void Inv_BuildWeaponList(int, int);

int BattleCmd_CommitAmmoAndUpdate(int arg0) {
    void *current;
    void *entry;
    int saved;
    int result;

    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            g_AyaHpCurrent = *(unsigned short *)((char *)current + 0xC);
            if (*(void **)((char *)current + 0x68) == 0) {
                goto tail;
            }
            saved = Inv_IsActiveListOverrideSelected();
            Inv_SelectActiveList(0);
            entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot);
            if (entry != 0) {
                *(short *)((char *)entry + 0xA) =
                    *(int *)((char *)*(void **)((char *)current + 0x68) + 0xC) & 0x3FF;
            }
            Inv_SelectActiveList(saved);
        }
    }

tail:
    result = Inv_DrawSlotItemIcon();
    Inv_BuildWeaponList(0, arg0);
    return result;
}
