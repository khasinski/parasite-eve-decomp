typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2360;
extern u16 D_800E2362;
extern u16 D_800E2364;
extern void *D_800E27A8;

int func_800CA7B8(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2360 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2362 = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2364 = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A8 = data;
    func_800CEDA8(0);
}
