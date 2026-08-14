/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* NOTE: cc.sh does not yet whitelist -mno-split-addresses; until it does,
 * build with PE_CC1_EXTRA_FLAGS=-mno-split-addresses. This flag makes
 * gcc 2.8.1 emit whole `la`/macro symbol addressing (like 2.7.2/ASPSX)
 * instead of separate %hi/%lo with CSE'd high parts. */
extern unsigned short g_SpuTransferAddr;
extern unsigned int _spu_mem_mode_plus;

int _spu_t();

int _spu_Fr(void *buf, int size) {
    _spu_t(2, g_SpuTransferAddr << _spu_mem_mode_plus);
    _spu_t(0);
    _spu_t(3, buf, size);
    return size;
}
