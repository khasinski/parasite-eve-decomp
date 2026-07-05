/* CC1_PROFILE: build0001 */
extern int g_SpuTransferStatus;
void Spu_ClearTransferCallback(void);
void SpuSetTransferCallback(void (*callback)(void));

/* CC1_PROFILE: build0001 */
void Spu_PrepareTransfer(void);
void Spu_UploadToSpu(int arg0, int arg1);

void Spu_PrepareTransfer(void) {
    g_SpuTransferStatus = 1;
    SpuSetTransferCallback(Spu_ClearTransferCallback);
}

void Spu_UploadWithPrepare(int arg0, int arg1) {
    Spu_PrepareTransfer();
    Spu_UploadToSpu(arg0, arg1);
}
