/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 */

extern void *D_800A1854;
extern int D_800A1858;

int MemCard_GetActiveProgressBlocks(void) {
    int blocks;
    int result;
    void *active = D_800A1854;

    if (active != 0) {
        blocks = (D_800A1858 - *(short *)((char *)active + 0x14) + 0x400) >> 10;
        result = 8;
        if (blocks < 9) {
            result = blocks;
        }
    } else {
        result = 0;
    }

    return result;
}
