/* MASPSX_FLAGS: --stack-return-delay */

extern int g_SpuTransferActiveFlag;
extern int _spu_transferCallback;

int _spu_Fw(int addr, int size);

unsigned int Spu_UploadToSpu(int arg0, unsigned int size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }
    _spu_Fw(arg0, size);
    if (_spu_transferCallback == 0) {
        g_SpuTransferActiveFlag = 0;
    }
    return size;
}
