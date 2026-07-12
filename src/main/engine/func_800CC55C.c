typedef unsigned char u8;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

extern int D_800E2270;
extern int D_800E2274;
extern int D_800E2278;
extern short D_800E2288;

int func_800CC55C(void *arg0, void *arg1, u8 *anim) {
    int i;
    u8 *entry;
    u8 *out;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800E2270 = *(short *)(anim + 0x6);
    D_800E2274 = *(short *)(anim + 0x6);
    D_800E2278 = *(short *)(anim + 0x6);
    D_800E2288 = anim[1];

    if ((signed char)anim[2] != 0) {
        i = 0;
        out = (u8 *)&D_800E2270 - 0x10;
        entry = anim;
        do {
            *(u16 *)(out + 0x0) = *(u16 *)(entry + 0x26);
            *(u16 *)(out + 0x2) = *(u16 *)(entry + 0x28);
            *(u16 *)(out + 0x4) = *(u16 *)(entry + 0x2A);
            i++;
            func_800C3B04(out);
            if ((unsigned int)i < (signed char)anim[2]) {
                entry += 8;
            }
        } while ((unsigned int)i < (signed char)anim[2]);
    }
}
