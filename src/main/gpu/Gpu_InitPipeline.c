/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
asm(".globl func_8003F074\nfunc_8003F074 = Gpu_InitPipeline");
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern s32 g_GlobalFrameCounter;
extern s32 g_GameStateFlags;
extern char D_8009D224[];
extern s32 g_SceneDispatchToken;
extern char g_TaskNodeSeqCounter[];
extern char g_GameState[];
extern char D_800B0CE4[];
extern char D_800B1628[];
extern char D_800B162C[];
extern char D_800B8A18[];
extern char g_ScreenTransitionState[];
extern char D_800BEA40[];
extern char D_800BEA42[];
extern char D_800BEA44[];
extern char D_800BEA46[];
extern char D_800BEA48[];
extern char D_800BEA4A[];
extern char D_800BEA4C[];
extern char D_800BEA4E[];
extern char D_800BEA50[];
extern char D_800942EC[];

extern s32 g_FieldMoveLock;

#define W(sym) (*(s32 *)(sym))
#define H(sym) (*(s16 *)(sym))
#define B(sym) (*(s8 *)(sym))
#define UB(sym) (*(u8 *)(sym))
#define LAUNDER(x) asm volatile("" : "=r"(x) : "0"(x))
#define BARRIER() asm volatile("" : : : "memory")

void Akao_LoadVoiceBankAlt(void);
void Scene_LoadRoom(int arg0);
s32 Overlay_StreamTexturePage(void);
void Entity_InitFreePool(void);
void Task_InitNodeFreeList(void);
void Gte_SetBackColor(void *arg0, int arg1, int arg2, int arg3);
void Gte_SetLightColor(void *arg0, int arg1, int arg2, int arg3, int arg4);
void Render_PrepareFrame(void);
void SetGeomScreen(int arg0);
int CdRom_DetectDiscChange(void);
int Scene_LoadEntityTexture(void);
void Scene_SetDiscSide(s8 arg0);
int Scene_LoadEntityTextures(void);
void Entity_RelocateSceneData(void);
void Render_SetupFogLayer(int arg0);
void Task_DrawSyncAndFlush(void);
void func_800E0060(void);
int DrawSync(int arg0);
void SetDispMask(int arg0);

void Gpu_InitPipeline(void) {
    int one;
    int v1;
    register int t0 asm("$8");
    register int t1 asm("$9");
    char *p;
    char *a0p;
    int geom;
    int frame_pad[6];

    Akao_LoadVoiceBankAlt();
    Scene_LoadRoom(g_SceneDispatchToken);
    Overlay_StreamTexturePage();
    W(D_8009D224) = 1;
    H(g_TaskNodeSeqCounter) = 1;
    Entity_InitFreePool();
    Task_InitNodeFreeList();

    Gte_SetBackColor(D_800BEA42 - 2, 0x28, 0x28, 0x28);
    BARRIER();

    v1 = 0;
    t1 = 0;
    LAUNDER(v1);
    if (v1 < -0x7FFF) {
        v1 = -0x7FFF;
    }
    t0 = 0x7FFF;
    a0p = D_800BEA40;
    *(s16 *)a0p = t0;
    H(D_800BEA42) = t1;
    H(D_800BEA44) = v1;
    Gte_SetLightColor(a0p, 0, 0xFF, 0xFF, 0xFF);

    v1 = 0;
    t0 = 0;
    LAUNDER(v1);
    t1 = 0x7FFF;
    if (v1 < -0x7FFF) {
        v1 = -0x7FFF;
    }
    p = D_800BEA46;
    *(s16 *)p = t0;
    H(D_800BEA48) = t1;
    H(D_800BEA4A) = v1;
    Gte_SetLightColor(p - 6, 1, 0x80, 0x80, 0x80);

    v1 = 0x10000;
    t0 = 0;
    {
        register int cond asm("$2");
        cond = 1;
        LAUNDER(cond);
        t1 = 0;
        if (cond != 0) {
            v1 = 0x7FFF;
        } else {
            LAUNDER(v1);
            if (v1 < -0x7FFF) {
                v1 = -0x7FFF;
            }
        }
    }
    p = D_800BEA4C;
    *(s16 *)p = t0;
    H(D_800BEA4E) = t1;
    H(D_800BEA50) = v1;
    Gte_SetLightColor(p - 0xC, 2, 0x60, 0x60, 0x60);

    one = 1;
    Render_PrepareFrame();
    SetGeomScreen(W(D_800B8A18));
    UB(g_ScreenTransitionState) |= 0x40;
    CdRom_DetectDiscChange();
    while (Scene_LoadEntityTexture() == one) {
    }

    one = 1;
    Scene_SetDiscSide(B(D_800B0CE4));
    while (Scene_LoadEntityTextures() == one) {
    }

    Entity_RelocateSceneData();
    if (W(g_GameState) & 0x40000000) {
        geom = W(D_800B162C);
    } else {
        geom = W(D_800B1628);
    }
    Render_SetupFogLayer(geom);
    Task_DrawSyncAndFlush();
    func_800E0060();
    g_GlobalFrameCounter = 0;
    H(D_800942EC) = 0;
    DrawSync(0);
    SetDispMask(1);
    {
        int tmp;
        v1 = ~0x40;
        tmp = g_GameStateFlags;
        a0p = g_GameState;
        g_GameStateFlags = tmp & v1;
    }
    asm volatile(
        "lui $2, %%hi(g_FieldMoveLock)\n"
        "lw $2, %%lo(g_FieldMoveLock)($2)\n"
        "addiu $3, $0, -13\n"
        "and $2, $2, $3\n"
        "lui $1, %%hi(g_FieldMoveLock)\n"
        "sw $2, %%lo(g_FieldMoveLock)($1)\n"
        :
        :
        : "$1", "$2", "$3", "memory");
    W(a0p) &= ~0x402;
}
