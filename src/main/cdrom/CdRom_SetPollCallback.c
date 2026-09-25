/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fcall-used-$1 */

extern unsigned int D_800A36A0;
extern unsigned int D_800A36A4;
extern unsigned int D_800A36A8;
extern unsigned int D_800A36AC;

void CdRom_SetPollCallback(unsigned int value) {
    D_800A36A0 = value;
}

void CdRom_SetSyncCallback(unsigned int value) {
    D_800A36A4 = value;
}

void CdRom_SetReadyCallback(unsigned int value) {
    D_800A36A8 = value;
}

void CdRom_SetDispatchCallback(unsigned int value) {
    D_800A36AC = value;
}
