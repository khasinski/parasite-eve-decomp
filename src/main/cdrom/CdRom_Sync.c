extern void CD_sync(int, int);

void CdRom_Sync(int result);

void CdRom_Sync(int result) {
    CD_sync(1, result);
}
