extern unsigned char D_80091A1C;
extern unsigned char g_MenuEquipSlotState;

int Menu_GetEquipSlotStateOrIndex(void) {
    if (D_80091A1C == 0) {
        return 1;
    }

    return g_MenuEquipSlotState;
}
