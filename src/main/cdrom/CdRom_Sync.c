extern void CD_sync(int, int);

void CdRom_Sync(int result) {
    CD_sync(1, result);
}

extern void CD_ready(int, int);

void CdRom_SendReadyCommand(int result) {
    CD_ready(1, result);
}
