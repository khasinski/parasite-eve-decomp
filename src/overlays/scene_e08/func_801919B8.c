typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern char D_8019880C;
extern s16 D_800F3374;

void *func_800C2B50(void);
void func_800C2EAC(u8 arg0);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C3134(void *arg0, s32 arg1, void *arg2);
s32 func_80077A64(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80077AA4(s32 arg0, s32 arg1);
void func_800D3114(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, void *arg10, void *arg11, s32 arg12);

void func_801919B8(void *arg0, void *arg1, char *arg2) {
    u8 scratch_a[8];
    u8 scratch_b[8];
    char *owner;
    register char *base __asm__("s0") = arg2;
    register char *state16 __asm__("s1");
    register char *state4 __asm__("s2");
    register char *draw __asm__("s3");
    register s32 i __asm__("s5");
    register s32 active __asm__("s4");
    register void *scratch_b_reg __asm__("s6");
    register void *resource __asm__("s7");
    register s32 tex_arg0 __asm__("a0");
    register s32 tex_arg1 __asm__("a1");
    s32 base_raw;
    s32 tex_x;

    owner = func_800C2B50();
    i = 0;
    resource = &D_8019880C;
    scratch_b_reg = scratch_b;
    __asm__ volatile("" : : "r"(resource), "r"(scratch_b_reg));
    draw = base;
    state16 = draw;
    __asm__ volatile("" : "=r"(state16) : "0"(state16));
    state4 = state16;
    func_800C2EAC(*(u8 *)(owner + 0x24));
    func_800C3098(0x10);
    func_800C3238(2);

    do {
        active = *(s16 *)(state16 + 0x1C0);
        if (active == 1) {
            func_800C3134(resource, *(s32 *)(state4 + 0x180), scratch_a);
            func_800C3134(resource, *(s32 *)(state4 + 0x180), scratch_b_reg);

            scratch_b[0] = 0;
            scratch_b[1] = 0;
            scratch_b[2] = 0;
            D_800F3374 = 0x1F4;

            base_raw = func_80077A64(0, 0, 0x340, 0x100);
            tex_arg0 = 0;
            tex_arg1 = 0x1D7;
            __asm__ volatile("" : : "r"(tex_arg0), "r"(tex_arg1));
            base = (char *)(base_raw & 0xFFFF);
            tex_x = func_80077AA4(tex_arg0, tex_arg1) & 0xFFFF;

            func_800D3114(draw, 2, *(s16 *)(state16 + 0x240), 0x40, 0x80, 0x9C, 0x10, (s32)base, tex_x, *(s16 *)(state16 + 0x200), scratch_a, scratch_b_reg, active);
        }

        draw += 0x18;
        state4 += 4;
        i++;
        state16 += 2;
    } while ((unsigned int)i < 16);
}
