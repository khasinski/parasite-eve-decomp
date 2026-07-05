/* CC1_PROFILE: build0001 */
void Spu_PrepareTransfer(void);
void Spu_ReadFromSpu(int arg0, int arg1);

extern volatile int g_SpuTransferStatus;

void Spu_ReadWithPrepare(int arg0, int arg1) {
    Spu_PrepareTransfer();
    Spu_ReadFromSpu(arg0, arg1);
}

void Spu_WaitTransferDone(void) {
    while (g_SpuTransferStatus == 1) {
    }
}
