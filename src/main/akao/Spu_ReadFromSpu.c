/* MASPSX_FLAGS: --stack-return-delay */

extern int g_SpuTransferActiveFlag;
extern int _spu_transferCallback;

int _spu_Fr(void *buf, int size);

unsigned int Spu_ReadFromSpu(int arg0, unsigned int size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }
    _spu_Fr(arg0, size);
    if (_spu_transferCallback == 0) {
        g_SpuTransferActiveFlag = 0;
    }
    return size;
}
