int MemCard_IsPortPresent(int arg0);

int Menu_IsMemCardSlotSelectable(int arg0) {
    int ret;

    ret = 0;
    if (arg0 == 2) {
        ret = 1;
    } else if (MemCard_IsPortPresent(arg0) != 0) {
        ret = 1;
    }
    return ret;
}
