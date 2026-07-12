typedef unsigned char u8;
typedef unsigned short u16;

int *func_800C2B10(int index);
int *func_800C2B28(int index);
void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3);
void func_800CEDA8(int arg0);

extern int D_800E2804;
extern u16 D_800E27F0;
extern u16 D_800E27F2;
extern u16 D_800E27F4;
extern u8 D_800B0CE8;
extern int D_800B0E14;

int func_800CD980(void) {
    int index;
    int *table;
    int *entry;
    int *next;
    u8 *data;

    index = *func_800C2B10(0xD);
    table = (int *)*func_800C2B28(1);
    entry = (int *)table[index];
    index++;
    next = (int *)table[index];
    D_800E2804 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (u8 *)D_800E2804;
    D_800E27F0 = *(u16 *)(data + 0x268);
    D_800E27F2 = *(u16 *)(data + 0x26A);
    D_800E27F4 = *(u16 *)(data + 0x26C);

    if (D_800B0CE8 != 0) {
        Akao_Cmd_24(D_800B0E14, 0, 0x80, 0x7F);
    }

    func_800CEDA8(0);
}
