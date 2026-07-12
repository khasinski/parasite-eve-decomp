typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2350;
extern u16 D_800E2352;
extern u16 D_800E2354;
extern void *D_800E27A0;

int func_800C8F28(char *obj) {
    char *data = *(char **)(obj + 8);
    char *model = *(char **)(data + 0x238);

    D_800E2350 = *(int *)(model + 0x274);
    D_800E2352 = *(int *)(model + 0x278);
    D_800E27A0 = data;
    D_800E2354 = *(int *)(model + 0x27C);
    func_800CEDA8(0);
}
