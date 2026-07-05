void Spu_PrepareTransfer(void);
unsigned int Spu_ReadFromSpu(int arg0, unsigned int size);

extern volatile int g_SpuTransferStatus;

void Spu_ReadWithPrepare(int arg0, int arg1) {
    Spu_PrepareTransfer();
    Spu_ReadFromSpu(arg0, arg1);
}

void Spu_WaitTransferDone(void) {
    while (g_SpuTransferStatus == 1) {
    }
}
