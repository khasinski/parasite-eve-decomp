extern void CD_ready(int, int);

void CdRom_SendReadyCommand(int result);

void CdRom_SendReadyCommand(int result) {
    CD_ready(1, result);
}
