typedef unsigned char u8;
typedef unsigned short u16;

int *func_800C2B10(int index);
int *func_800C2B28(int index);
void func_800CEDA8(int arg0);

extern int D_800E2848;
extern u16 D_800E2808;
extern u16 D_800E280A;
extern u16 D_800E280C;

int func_800CE1FC(void) {
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
    D_800E2848 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (u8 *)D_800E2848;
    D_800E2808 = *(u16 *)(data + 0x268);
    D_800E280A = *(u16 *)(data + 0x26A);
    D_800E280C = *(u16 *)(data + 0x26C);

    func_800CEDA8(0);
}
