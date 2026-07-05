/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
int MenuWidget_GridCellIndex(void *node);
void Inv_RemoveActiveListItem(int arg0);

typedef signed char s8;
typedef unsigned short u16;

extern u16 *g_InvItemPtr;
extern s8 g_AyaEquippedWeaponSlot[];
extern s8 g_AyaEquippedArmorSlot[];

void Inv_RebuildSelectableMask(void);

void Inv_DropCurrentSelectionItem(void) {
    void *node = MenuWidget_FindByModeAndSelectedBase(2, 1);
    int index;

    if (node != 0) {
        index = MenuWidget_GridCellIndex(node);
        if (index >= 0) {
            Inv_RemoveActiveListItem(index);
        }
    }
}

int Inv_SwapSlots(int unused, int from, int unused2, int to) {
    g_InvItemPtr[from] ^= g_InvItemPtr[to];
    g_InvItemPtr[to] ^= g_InvItemPtr[from];
    g_InvItemPtr[from] ^= g_InvItemPtr[to];

    if (g_AyaEquippedWeaponSlot[0] == from) {
        g_AyaEquippedWeaponSlot[0] = to;
    } else if (g_AyaEquippedWeaponSlot[0] == to) {
        g_AyaEquippedWeaponSlot[0] = from;
    }

    if (g_AyaEquippedArmorSlot[0] == from) {
        g_AyaEquippedArmorSlot[0] = to;
    } else if (g_AyaEquippedArmorSlot[0] == to) {
        g_AyaEquippedArmorSlot[0] = from;
    }

    Inv_RebuildSelectableMask();
    return 1;
}
