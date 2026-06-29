/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef short s16;

extern s16 *g_InvItemPtr;

asm(".globl func_80057D18");
asm("func_80057D18 = Inv_ClearActiveListSlot");

int Inv_ClearActiveListSlot(int arg0) {
    int value;

    value = g_InvItemPtr[arg0];
    g_InvItemPtr[arg0] = 0;
    return value;
}
