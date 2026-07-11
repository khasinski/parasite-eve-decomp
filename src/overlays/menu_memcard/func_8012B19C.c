extern unsigned char D_800B0DBA;

void func_800870F0(int arg0);
void func_8010C0D8(int arg0);
void func_8007A2A4(void);
void func_80080DC4(int arg0, int arg1, int arg2);

void func_8012B19C(void) {
    unsigned char *counter;
    int value;

    counter = &D_800B0DBA;
    value = *counter - 1;
    *counter = value;
    func_800870F0(0);
    func_8010C0D8(0);
    func_8007A2A4();
    func_80080DC4(9, 0, 0);
}
