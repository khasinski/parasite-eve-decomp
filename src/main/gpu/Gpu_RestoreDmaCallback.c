/* CC1_FLAGS: -g3 */

void Gpu_SetDisplayBuffer(void);
void DMACallback(int arg0, void (*arg1)(void));

void Gpu_RestoreDmaCallback(void) {
    DMACallback(2, Gpu_SetDisplayBuffer);
}
