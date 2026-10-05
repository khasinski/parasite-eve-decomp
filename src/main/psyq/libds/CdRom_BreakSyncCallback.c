void StCdInterrupt(void);

void CdRom_BreakSyncCallback(unsigned char event, unsigned char *result) {
    StCdInterrupt();
}
