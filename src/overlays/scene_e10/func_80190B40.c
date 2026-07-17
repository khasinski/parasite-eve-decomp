char *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C5A40(void *arg0);

void func_80190B40(void *arg0, void *arg1, char *state) {
    char *obj;

    obj = func_800C2B50();
    func_800C2EAC((unsigned char)obj[0x6C]);
    func_800C2FF0(0x10, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C5A40(state + 0xAC);
    func_800C5A40(state + 0x78);
}
