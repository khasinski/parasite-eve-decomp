void Render_InitParticlePool(void);

void CdRom_BreakSyncCallback(unsigned char event, unsigned char *result) {
    Render_InitParticlePool();
}
