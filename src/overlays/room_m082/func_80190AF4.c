typedef unsigned char u8;

extern char *func_800C2B50(void);
extern void func_800C2EAC(int arg0);
extern void func_800C2FF0(int arg0, int arg1);
extern void func_800C3098(int arg0);
extern void func_800C3238(int arg0);
extern void func_800C5A40(void *arg0);

void func_80190AF4(void *arg0, void *arg1, char *sys) {
    register char *sysp asm("s0") = sys;
    char *root;

    root = func_800C2B50();
    func_800C2EAC((u8)root[0x6C]);
    func_800C2FF0(0x10, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C5A40(sysp + 0xAC);
    func_800C5A40(sysp + 0x78);
}
