typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2358;
extern u16 D_800E235A;
extern u16 D_800E235C;
extern void *D_800E27A4;

int func_800C9C20(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2358 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E235A = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E235C = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A4 = data;
    func_800CEDA8(0);
}
