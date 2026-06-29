/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int g_InvSortRankTableA[];
extern int D_80092410[];
extern int g_InvTypeRankTableA[];
extern int g_InvItemArray[];
extern int D_8009D0A4;
extern int D_8009D0A8;
extern int g_InvSortListBase;
extern int g_InvSortListCount;
extern int g_InvLookupPtr;
extern int g_InvSortRankTable;
extern int g_InvSortTypeRankTable;

void Inv_LookupData(void);
void Inv_CompareItemsForSort(void);
void qsort(int *arg0, int arg1, int arg2, void (*arg3)(void));
void Inv_SortWeaponSubrange(void);
void Inv_SortAmmoSubrange(void);
void Inv_SortInventoryByMode(int arg0, int arg1);
void Inv_TransferItemAlt2(int arg0);

void Sort_InventoryItems(int arg0, int arg1) {
    if (arg0 != 0) {
        D_8009D0A4 = arg1;
        g_InvSortRankTable = (int)g_InvSortRankTableA;
    } else {
        D_8009D0A8 = arg1;
        g_InvSortRankTable = (int)D_80092410;
    }

    g_InvSortListBase = (int)g_InvItemArray;
    g_InvSortTypeRankTable = (int)g_InvTypeRankTableA;
    g_InvSortListCount = 0x52;
    g_InvLookupPtr = (int)Inv_LookupData;
    qsort(g_InvItemArray, 0x52, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(arg0, arg1);
    Inv_TransferItemAlt2(0x3803FE);
}
