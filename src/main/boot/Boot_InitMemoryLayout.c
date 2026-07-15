
typedef int s32;

extern char D_800F34F8[];
extern char D_8010BD00[];
extern char D_80120D08[];
extern char D_801ED800[];
extern s32 g_StrFileDirBuffer;

extern volatile s32 D_800B0E24;
extern volatile s32 D_800B0E28;
extern volatile s32 D_800B0E2C;
extern volatile s32 D_800B0E30;
extern volatile s32 D_800B0E34;
extern volatile s32 g_OtBufferTable;
extern volatile s32 g_RenderOtBufferBaseAlt;
extern volatile s32 D_800B0E40;
extern volatile s32 D_800B0E44;
extern volatile s32 D_800B0E48;
extern volatile s32 g_RenderScratchBufferBase;
extern volatile s32 g_RenderFrontBufferBase;
extern volatile s32 g_RenderBackBufferBase;
extern volatile s32 g_RenderAnimBufferBase;
extern volatile s32 D_800B0E5C;
extern volatile s32 D_800B0E60;
extern volatile s32 g_LoadedSceneAssetBlock;
extern volatile s32 D_800B0E68;
extern volatile s32 g_SceneLoadScratchBuffer;

void Boot_InitMemoryLayout(void) {
    register s32 ptr asm("$2");
    s32 ptr2;
    register s32 const_8000 asm("$4");
    s32 const_48000;
    s32 tmp;

    const_48000 = 0x48000;

    ptr = (s32)D_800F34F8;
    ptr2 = ptr + 0x1800;
    D_800B0E24 = ptr;
    ptr += 0x6000;
    D_800B0E28 = ptr2;
    D_800B0E2C = ptr;
    ptr += 0xE000;
    D_800B0E30 = ptr;

    ptr = (s32)D_8010BD00;
    ptr2 = (s32)D_80120D08;
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
    ptr += const_48000;
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
    g_SceneLoadScratchBuffer = (s32)D_801ED800;
    g_LoadedSceneAssetBlock = tmp - 8;
    D_800B0E68 = tmp;
}
