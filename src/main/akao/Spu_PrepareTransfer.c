/* CC1_PROFILE: build0001 */
extern int g_SpuTransferStatus;
void Spu_ClearTransferCallback(void);
void SpuSetTransferCallback(void (*callback)(void));

void Spu_PrepareTransfer(void) {
    g_SpuTransferStatus = 1;
    SpuSetTransferCallback(Spu_ClearTransferCallback);
}
