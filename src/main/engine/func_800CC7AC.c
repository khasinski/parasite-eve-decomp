typedef unsigned char u8;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

extern short D_800E2840;

int func_800CC7AC(void *arg0, void *arg1, u8 *anim) {
    int i;
    u8 *entry;
    u8 *out;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    D_800E2840 = (signed char)anim[3] * 2;

    if (*(short *)(anim + 0x4) != 0) {
        i = 0;
        out = (u8 *)&D_800E2840 - 0x28;
        entry = anim;
        do {
            *(u16 *)(out + 0x0) = *(u16 *)(entry + 0x8);
            *(u16 *)(out + 0x2) = *(u16 *)(entry + 0x28);
            *(u16 *)(out + 0x4) = *(u16 *)(entry + 0x48);
            i++;
            func_800C3B04(out);
            if ((unsigned int)i < *(short *)(anim + 0x4)) {
                entry += 2;
            }
        } while ((unsigned int)i < *(short *)(anim + 0x4));
    }
}
