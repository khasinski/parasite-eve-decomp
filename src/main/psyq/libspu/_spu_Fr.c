/* MASPSX_FLAGS: --stack-return-delay */
extern unsigned short g_SpuTransferAddr;
extern unsigned int _spu_mem_mode_plus;

int _spu_t();

int _spu_Fr(void *buf, int size) {
    _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
    _spu_t(0);
    _spu_t(3, buf, size);
    return size;
}
