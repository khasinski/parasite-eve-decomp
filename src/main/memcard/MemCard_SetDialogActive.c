/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009D030;
extern unsigned int D_800B0CD8[];

void MemCard_CloseAll(void);

void MemCard_SetDialogActive(int active) {
    if (active == 0) {
        D_8009D030 = 0;
        MemCard_CloseAll();
        D_800B0CD8[0] &= 0xFFFF3FFF;
    } else if (D_8009D030 == 0) {
        D_8009D030 = 1;
        D_800B0CD8[0] |= 0xC000;
    }
}
