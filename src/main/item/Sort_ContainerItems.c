/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern int g_InvSortRankTableA[];
extern int D_80092410[];
extern int g_InvTypeRankTableA[];
extern int g_InvTypeRankTableB[];
extern int g_WayneStorageItems[];
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

void Sort_ContainerItems(int arg0) {
    if (arg0 != 0) {
        g_InvSortRankTable = (int)g_InvSortRankTableA;
        g_InvSortTypeRankTable = (int)g_InvTypeRankTableA;
    } else {
        g_InvSortRankTable = (int)D_80092410;
        g_InvSortTypeRankTable = (int)g_InvTypeRankTableB;
    }

    g_InvSortListBase = (int)g_WayneStorageItems;
    g_InvSortListCount = 0x64;
    g_InvLookupPtr = (int)Inv_LookupData;
    qsort(g_WayneStorageItems, 0x64, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(2, arg0);
    Inv_TransferItemAlt2(0xF400);
}
