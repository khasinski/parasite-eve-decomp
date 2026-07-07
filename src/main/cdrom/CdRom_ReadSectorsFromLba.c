/* CC1_FLAGS: -g3 */

int CdRom_ReadSectors(int lba, int offset, int dst, int size);

void CdRom_ReadSectorsFromLba(int arg0, int arg1, int arg2) {
    CdRom_ReadSectors(arg0, 0, arg1, arg2);
}
