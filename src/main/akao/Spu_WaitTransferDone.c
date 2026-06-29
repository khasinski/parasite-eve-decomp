extern volatile int g_SpuTransferStatus;

void Spu_WaitTransferDone(void) {
    while (g_SpuTransferStatus == 1) {
    }
}
