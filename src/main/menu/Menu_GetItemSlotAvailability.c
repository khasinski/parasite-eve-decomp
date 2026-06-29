int Inv_GetPackedListCount(void);
int Inv_GetPackedListItem(int arg0);
int Menu_CheckItemAffordable(int arg0);

int Menu_GetItemSlotAvailability(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        return Menu_CheckItemAffordable(Inv_GetPackedListItem(arg0));
    }

    return 0;
}
