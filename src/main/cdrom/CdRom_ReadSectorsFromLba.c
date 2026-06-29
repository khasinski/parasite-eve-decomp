/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

void CdRom_ReadSectors(int arg0, int arg1, int arg2, int arg3);

void CdRom_ReadSectorsFromLba(int arg0, int arg1, int arg2) {
    CdRom_ReadSectors(arg0, 0, arg1, arg2);
}
