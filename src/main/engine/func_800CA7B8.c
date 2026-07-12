typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;
extern void *D_800E27A8;

int func_800CA7B8(char *obj) {
    char *data = *(char **)(obj + 8);
    char *model = *(char **)(data + 0x238);

    D_800E2360 = *(int *)(model + 0x274);
    D_800E2362 = *(int *)(model + 0x278);
    D_800E27A8 = data;
    D_800E2364 = *(int *)(model + 0x27C);
    func_800CEDA8(0);
}
