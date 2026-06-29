/* CC1_PROFILE: build0001 */
extern int g_SpuTransferStatus;
void SpuSetTransferCallback(void (*callback)(void));

void Spu_ClearTransferCallback(void) {
    SpuSetTransferCallback(0);
    g_SpuTransferStatus = 0;
}
