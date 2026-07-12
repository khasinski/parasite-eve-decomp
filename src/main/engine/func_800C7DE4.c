typedef unsigned short u16;

void func_800CEDA8(int arg0);

extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;
extern void *D_800E279C;

int func_800C7DE4(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2348 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E234A = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E234C = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E279C = data;
    func_800CEDA8(0);
}
