/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: --stack-return-delay */
extern int D_8009B418;
extern unsigned short g_SpuTransferAddr;
extern int _spu_mem_mode_plus;

int _spu_t(int cmd, ...);
void _spu_FwriteByIO(int addr, int size);

int _spu_Fw(int addr, int size) {
    if (D_8009B418 == 0) {
        _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
        _spu_t(1);
        _spu_t(3, addr, size);
    } else {
        _spu_FwriteByIO(addr, size);
    }

    return size;
}
