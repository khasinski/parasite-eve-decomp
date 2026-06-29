typedef unsigned short u16;

extern u16 g_InvCategoryItemTable[];

int Inv_GetWeaponCategoryAmmoBase(unsigned int arg0) {
    if (arg0 >= 3) {
        return 0;
    }
    return g_InvCategoryItemTable[arg0 * 0x10];
}
