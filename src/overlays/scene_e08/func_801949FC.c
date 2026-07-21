typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct { s16 vx, vy, vz, pad; } SVEC;
typedef struct { s32 vx, vy, vz, pad; } VEC;
typedef struct { SVEC sv; u8 pad[0xC]; s32 f14, f18, f1C; } Buf;

s32 func_800C2B50(void);
void func_800C2EAC(s32);
void func_800C2FF0(s32, s32);
void func_800C3098(s32);
void func_800C3238(s32);
void func_800794C4(void *arg0, void *arg1);
void func_80071A44(VEC *arg0, s32 arg1, s32 arg2);
void func_80078CC4(void *dst, VEC *src);
void func_800C3134(u8 *table, s32 step, u8 *out);
void func_800C42A4(void *arg0, void *arg1, s32 arg2);

extern u8 D_80198AD4;
extern u8 D_80199570;
extern u8 D_80199574;
extern u16 D_8019957A;

void func_801949FC(s32 arg0, char *arg1, char *rec) {
    s32 ctx;
    u32 idx;
    char *r;
    Buf buf;
    VEC vdst;
    VEC vsrc;

    r = rec;
    ctx = func_800C2B50();
    idx = ((*(u8 *)(rec + 0x22) >> 4) << 1) << 1;
    if (idx >= 0xD) {
        idx = 0xC;
    }
    func_800C2EAC(*(u8 *)(ctx + 0x24));
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800794C4(r + 0x10, &buf);
    func_80071A44(&vsrc, 0, 0x10);
    vsrc.vx = *(s16 *)(r + 0x1C);
    vsrc.vy = *(s16 *)(r + 0x1C);
    vsrc.vz = 0x1000;
    vdst = vsrc;
    func_80078CC4(&buf, &vdst);
    func_800C3134(&D_80198AD4, *(s16 *)(arg1 + 2), &D_80199570);
    buf.f14 = *(s32 *)(r + 0);
    buf.f18 = *(s32 *)(r + 4);
    buf.f1C = *(s32 *)(r + 8);
    D_80199574 = idx;
    D_8019957A = *(u16 *)(r + 0x20);
    func_800C42A4(&D_80199570, &buf, 1);
}
