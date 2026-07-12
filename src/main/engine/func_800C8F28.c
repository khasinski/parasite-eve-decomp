typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2350;
extern u16 D_800E2352;
extern u16 D_800E2354;
extern void *D_800E27A0;

int func_800C8F28(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2350 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2352 = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2354 = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A0 = data;
    func_800CEDA8(0);
}
