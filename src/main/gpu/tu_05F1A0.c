/* CC1_PROFILE: build0001 */

typedef short s16;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

void VSync(int arg0);
void SetDispMask(int arg0);
void PutDispEnv(void *env) __asm__("Render_StepEntityPool");
void ClearImage(RECT *rect, int r, int g, int b);
void DrawSync(int arg0);
void Draw_InitBuffers(void);
void CdRom_SetSeekPos(int arg0);
void ClearOTagR(int arg0, int arg1);
void Render_SetCDDCSlot(void);
void Gpu_RenderFrame(void);
void Menu_ConsumeEquipSlotFlag(void);

extern char g_RenderDispEnvArray[];
extern char D_800F34F8[];
extern char D_8010BD00[];
extern char D_80120D08[];
extern char D_801ED800[];
extern int g_StrFileDirBuffer;
extern int g_ActiveDrawSlot;
extern unsigned char g_ScreenTransitionState;
extern volatile unsigned char D_800B0DC6;
extern volatile int g_SceneDispatchToken;

extern volatile int D_800B0E24;
extern volatile int D_800B0E28;
extern volatile int D_800B0E2C;
extern volatile int D_800B0E30;
extern volatile int D_800B0E34;
extern volatile int g_OtBufferTable;
extern volatile int g_RenderOtBufferBaseAlt;
extern volatile int D_800B0E40;
extern volatile int D_800B0E44;
extern volatile int D_800B0E48;
extern volatile int g_RenderScratchBufferBase;
extern volatile int g_RenderFrontBufferBase;
extern volatile int g_RenderBackBufferBase;
extern volatile int g_RenderAnimBufferBase;
extern volatile int D_800B0E5C;
extern volatile int D_800B0E60;
extern volatile int g_LoadedSceneAssetBlock;
extern volatile int D_800B0E68;
extern volatile int g_SceneLoadScratchBuffer;

#define LAUNDER2(a, b) asm volatile("" : "=r"(a), "=r"(b) : "0"(a), "1"(b))

extern signed char g_DrawEnabled;

int Gpu_InitDisplay(int mode) {
    int mode_reg;
    int one;
    register volatile int *slots asm("$18");
    register int ptr asm("$2");
    int ptr2;
    register int const_8000 asm("$4");
    int const_48000;
    RECT rect;
    int tmp;

    mode_reg = mode;
    VSync(0);
    SetDispMask(0);
    PutDispEnv(g_RenderDispEnvArray);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0x1C0;
    one = 1;
    ClearImage(&rect, 0, 0, one);
    DrawSync(0);

    const_48000 = 0x48000;
    slots = &g_OtBufferTable;
    LAUNDER2(const_48000, slots);

    ptr = (int)D_800F34F8;
    ptr2 = ptr + 0x1800;
    D_800B0E24 = ptr;
    ptr += 0x6000;
    D_800B0E28 = ptr2;
    D_800B0E2C = ptr;
    ptr += 0xE000;
    D_800B0E30 = ptr;

    ptr = (int)D_8010BD00;
    ptr2 = (int)D_80120D08;
    D_800B0E40 = ptr;
    ptr = ptr2 + 0x1C98;
    D_800B0E34 = ptr2;
    ptr2 += 0x5C98;
    const_8000 = 0x8000;
    g_OtBufferTable = ptr;
    ptr += const_8000;
    g_RenderOtBufferBaseAlt = ptr2;
    ptr2 = ptr + 0x2400;
    D_800B0E44 = ptr;
    ptr += 0x4800;
    g_RenderScratchBufferBase = ptr;
    asm("addu %0,%1,%2" : "=r"(ptr) : "r"(ptr), "r"(const_48000));
    D_800B0E48 = ptr2;
    ptr2 = ptr + 0x4000;
    g_RenderFrontBufferBase = ptr;
    ptr += const_8000;
    g_RenderBackBufferBase = ptr2;
    ptr2 = ptr + 0x3800;
    D_800B0E5C = ptr2;
    tmp = g_StrFileDirBuffer;
    g_RenderAnimBufferBase = ptr;
    ptr += 0x7000;
    D_800B0E60 = ptr;
    g_SceneLoadScratchBuffer = (int)D_801ED800;
    g_LoadedSceneAssetBlock = tmp - 8;
    D_800B0E68 = tmp;

    Draw_InitBuffers();
    CdRom_SetSeekPos(2);

    do {
        ClearOTagR(slots[g_ActiveDrawSlot], 0x1000);
        Render_SetCDDCSlot();
        Gpu_RenderFrame();
    } while ((g_ScreenTransitionState & 3) != one);

    D_800B0DC6 = 0;
    Menu_ConsumeEquipSlotFlag();

    if (mode_reg == 1) {
        g_SceneDispatchToken = 0xA80830C8;
    } else if (mode_reg == 3) {
        g_SceneDispatchToken = 0xA80651C8;
    }
    return 0;
}

int Draw_GetDrawEnabled(void) {
    return g_DrawEnabled;
}
