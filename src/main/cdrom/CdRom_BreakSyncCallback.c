void Render_InitParticlePool(void);

void CdRom_BreakSyncCallback(void) {
    Render_InitParticlePool();
}
