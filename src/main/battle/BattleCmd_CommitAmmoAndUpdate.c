typedef signed char s8;

extern void **g_PlayerEntity __asm__("D_8009D254");
extern short g_AyaHpCurrent __asm__("D_800C0E08");
extern s8 g_AyaEquippedWeaponSlot __asm__("D_800C0E20");

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int mode);
void *Inv_LookupActiveListData(int slot);
int Inv_DrawSlotItemIcon(void);
void Inv_BuildWeaponList(int unused, void *out);

int BattleCmd_CommitAmmoAndUpdate(void *out) {
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
    Inv_BuildWeaponList(0, out);
    return result;
}
