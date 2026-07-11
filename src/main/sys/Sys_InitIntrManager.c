/* MASPSX_FLAGS: --stack-return-delay */
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

extern u16 D_800945E4[];
extern u16 D_80094620[];
extern u32 *D_8009566C;
extern u16 *D_80095670;
extern volatile u16 *D_80095674;
extern u32 *D_80095678;

void Sys_ClearWordBuf(void *dst, int count);
int RawData_80074354(void *dst);
void Render_InitSceneGeom(void);
void HookEntryInt(void *entry);
int startIntrVSync(void);
int startIntrDMA(void);
void Sys_HleJumpA0(void *arg);
void ExitCriticalSection(void);

u16 *Sys_InitIntrManager(void) {
    register u16 *state asm("$16");
    u16 *dst;
    volatile u16 *src;
    u16 *flag;
    u32 *ptr;
    register u32 *vsync_ptr asm("$3");
    int ret;

    state = D_800945E4;
    if (state[0] != 0) {
        return 0;
    }

    dst = D_80095670;
    src = D_80095674;
    *src = 0;
    *dst = *src;
    *D_80095678 = 0x33333333;

    Sys_ClearWordBuf(state, 0x41A);
    if (RawData_80074354(state + 0x1C) != 0) {
        Render_InitSceneGeom();
    }

    state = D_80094620;
    *((u32 *)state) = (u32)(state + 0x7EE);
    HookEntryInt(state - 2);
    flag = state - 0x1E;
    *flag = 1;

    ret = startIntrVSync();
    vsync_ptr = D_8009566C;
    vsync_ptr[5] = ret;

    ret = startIntrDMA();
    ptr = D_8009566C;
    ptr[1] = ret;

    Sys_HleJumpA0(D_8009566C);
    state = flag;
    ExitCriticalSection();

    return state;
}
