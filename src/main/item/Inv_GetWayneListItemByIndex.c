/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
typedef short s16;
typedef int s32;

extern s32 D_8009D044;
extern s16 D_800A1E00[];

int Inv_GetWayneListItemByIndex(int index) {
    if (index >= 0 && index < D_8009D044) {
        return D_800A1E00[index];
    }
    return 0;
}
