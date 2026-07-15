/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

typedef signed char s8;
typedef short s16;
typedef unsigned int u32;

extern u32 *D_8009D014;
extern u32 D_800A1AA0[];
extern s16 D_800C0E48[];
extern s8 D_800C0E20[];
extern s8 D_800C0E22[];

int Inv_CheckSlotUsable(int data);
int Inv_FindIndexByData(int data);

void BattleCmd_UndoPending(void) {
    register u32 *entry asm("$4");
    register u32 *entry_tmp asm("$2");
    register u32 *top asm("$3");
    int opcode;
    s16 *slot;
    int data;
    int index;

    top = D_8009D014;
    if (D_800A1AA0 < top) {
        entry_tmp = top - 9;
        opcode = top[-9];
        D_8009D014 = entry_tmp;
        asm volatile("" : "=r"(entry) : "0"(entry_tmp));

        switch (opcode) {
        case 0:
            slot = &D_800C0E48[entry[2]];
            if (*slot == 0) {
                *slot = entry[1];
            } else {
                Inv_CheckSlotUsable(entry[1]);
            }
            break;

        case 1:
            break;

        case 2:
            D_800C0E20[0] = Inv_FindIndexByData(entry[1]);
            break;

        case 3:
            data = entry[1];
            index = -1;
            if (data != 0) {
                index = Inv_FindIndexByData(data);
            }
            D_800C0E22[0] = index;
            break;

        case 4:
            *(s16 *)(entry[1] + 0xA) = entry[5];
            *(s16 *)(entry[2] + 0xA) = entry[7];
            *(s16 *)(entry[3] + 0xA) = entry[8];
            break;
        }
    }
}
