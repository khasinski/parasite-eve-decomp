typedef signed char s8;

extern void **D_8009D254;
extern short D_800C0E08;
extern s8 D_800C0E20;

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

    if (D_8009D254 != 0) {
        current = D_8009D254[0];
        if (current != 0) {
            D_800C0E08 = *(unsigned short *)((char *)current + 0xC);
            if (*(void **)((char *)current + 0x68) == 0) {
                goto tail;
            }
            saved = Inv_IsActiveListOverrideSelected();
            Inv_SelectActiveList(0);
            entry = Inv_LookupActiveListData(D_800C0E20);
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
