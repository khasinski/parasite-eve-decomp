typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;
extern void *D_800E279C;

int func_800C7DE4(char *obj) {
    char *data = *(char **)(obj + 8);
    char *model = *(char **)(data + 0x238);

    D_800E2348 = *(int *)(model + 0x274);
    D_800E234A = *(int *)(model + 0x278);
    D_800E279C = data;
    D_800E234C = *(int *)(model + 0x27C);
    func_800CEDA8(0);
}
