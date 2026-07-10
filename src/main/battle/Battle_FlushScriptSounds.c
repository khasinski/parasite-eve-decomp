/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned char D_8009D1D4;
extern unsigned char D_8009CE3C;
extern unsigned short D_800BE834[];
extern struct { char _[16]; } D_8009D1A0_r __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D1A0_w __asm__("D_8009D1A0");

#define D_8009D1A0_R (*(int *)&D_8009D1A0_r)
#define D_8009D1A0_W (*(int *)&D_8009D1A0_w)

int Inv_CheckSlotUsable(int slot);

void Battle_FlushScriptSounds(void) {
    register unsigned int index asm("$16");
    int flags;
    int mask;
    int item;

    index = D_8009D1D4;
    while ((index & 0xFF) < D_8009CE3C) {
        item = *(unsigned short *)((char *)D_800BE834 + ((index & 0xFF) << 3));
        if ((unsigned int)(item - 3) < 0x180) {
            Inv_CheckSlotUsable((short)item - 3);
        }

        index++;
    }

    flags = D_8009D1A0_R;
    mask = -0x101;
    D_8009CE3C = 0;
    D_8009D1D4 = 0;
    D_8009D1A0_W = flags & mask;
}
