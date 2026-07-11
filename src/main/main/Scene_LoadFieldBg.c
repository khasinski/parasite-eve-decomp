/* Scene_LoadFieldBg (0x8006AD40): field room asset loader. Runs only when
 * (g_GameState[0] & 1) is set, streams room sections from PE.IMG, uploads TIM
 * texture pages, resolves room subresources, and resets render/entity state. */
typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

extern struct { char _[16]; } g_GameState_o __asm__("g_GameState");
extern struct { char _[16]; } g_PeImageBaseLba_o __asm__("g_PeImageBaseLba");
extern struct { char _[16]; } D_800930EA_o __asm__("D_800930EA");
extern struct { char _[16]; } D_800930EC_o __asm__("D_800930EC");
extern struct { char _[16]; } D_800930EE_o __asm__("D_800930EE");
extern struct { char _[16]; } D_800930F0_o __asm__("D_800930F0");
extern struct { char _[16]; } D_800930E0_o __asm__("D_800930E0");
extern struct { char _[16]; } D_80093126_o __asm__("D_80093126");
extern struct { char _[16]; } D_80091648_o __asm__("D_80091648");
extern s32 D_8009CDDC;
extern u8 D_800BCE80[];

#define GAME ((u8 *)&g_GameState_o)
#define D_80091648 ((u8 *)&D_80091648_o)
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

int CdRom_ReadSectorsFromLba(int lba, void *dst, int sectors);
int CdRom_PollReady(void);
int Gpu_LoadTimAsset(void *tim, void *base);
int Gpu_LoadTimImage(void *tim);
int Asset_FindTable08ByU32Key(void *base, int key);
int LoadImage(RECT *rect, void *pixels);
void Akao_Cmd_F1(void);
int DrawSync(int mode);
void Render_InitSceneGeom(void);
void Render_InitEntityPool(int mode);
void Render_StepEntityPool(void *env);
void SetDispMask(int enabled);
void Battle_DrawHPBar(void);

static void Scene_LoadFieldBgReadRange(u8 *range, void *dst) {
    int start;
    int end;

    start = U16_AT(range, 0);
    end = U16_AT(range, 2);
    while (CdRom_ReadSectorsFromLba(*(s32 *)&g_PeImageBaseLba_o + start, dst,
                                    end - start) == -1) {}
}

static int Scene_LoadFieldBgPollOrRetry(void) {
    int status;

    do {
        status = CdRom_PollReady();
        if (status == -1) {
            return -1;
        }
    } while (status != 0);

    return 0;
}

static void Scene_LoadFieldBgLoadTimList(u8 *room) {
    u8 *header;
    u8 *entry;
    u32 packed;
    int i;
    int count;

    header = room + U32_AT(room, 4);
    packed = U32_AT(header, 0x28);
    count = packed >> 22;
    entry = room + (packed & 0x3FFFFF);
    for (i = 0; i < count; i++) {
        Gpu_LoadTimAsset(entry, room);
        entry += 0x14;
    }
}

static void Scene_LoadFieldBgPackTextureDescriptors(int start, int end) {
    int off;
    u16 x;
    u16 y;
    u16 clut_x;
    u16 clut_y;
    u16 tpage;
    u16 clut;

    for (off = start; (unsigned int)off < (unsigned int)end; off += 0x10) {
        x = U16_AT(D_80091648, off + 0);
        y = U16_AT(D_80091648, off + 2);
        clut_x = U16_AT(D_80091648, off + 4);
        clut_y = U16_AT(D_80091648, off + 6);

        tpage = ((y & 0x100) >> 4) | (((x & 0x3FF) >> 6) | 0x20) |
                ((y & 0x200) << 2);
        clut = (clut_y << 6) | ((clut_x >> 4) & 0x3F);

        U16_AT(D_80091648, off + 8) = tpage;
        U16_AT(D_80091648, off + 0x0A) = clut;
    }
}

static void Scene_LoadFieldBgLoadImages(u8 *room) {
    u8 *entry;
    u32 size;

    entry = (u8 *)Asset_FindTable08ByU32Key(room, 0xABADC06C);
    while ((size = U32_AT(entry, 0)) != 0) {
        LoadImage((RECT *)(entry + 4), entry + 0x0C);
        entry += (size >> 2) << 2;
    }
}

static void Scene_LoadFieldBgResolveSubresources(u8 *room) {
    PTR_AT(GAME, 0x11C) = (u8 *)Asset_FindTable08ByU32Key(room, 0xC4B5BA04);
    PTR_AT(GAME, 0x120) = (u8 *)Asset_FindTable08ByU32Key(room, 0xCAAD0704);
    PTR_AT(GAME, 0x124) = (u8 *)Asset_FindTable08ByU32Key(room, 0x5EAF6804);
}

static void Scene_LoadFieldBgFinish(void) {
    u32 flags;

    Akao_Cmd_F1();
    DrawSync(0);
    Render_InitEntityPool(1);
    Render_InitSceneGeom();
    Render_StepEntityPool(D_800BCE80 + D_8009CDDC * 0x14);
    SetDispMask(1);

    S16_AT(GAME, 6) = -1;
    U8_AT(GAME, 0x0B) = 0;
    S8_AT(GAME, 0x0C) = -1;
    S8_AT(GAME, 9) = -1;
    S16_AT(GAME, 0xE8) = -1;
    U8_AT(GAME, 0xEB) = 0;

    flags = U32_AT(GAME, 0);
    U8_AT(GAME, 0xEA) = 0;
    if (!(flags & 0x40)) {
        S8_AT(GAME, 0xDA) = -1;
        S8_AT(GAME, 0xDD) = -1;
        S8_AT(GAME, 0xDC) = -1;
    }

    if (!(U32_AT(GAME, 0) & 0x80)) {
        S8_AT(GAME, 0xDB) = -1;
        S8_AT(GAME, 0xDF) = -1;
        S8_AT(GAME, 0xDE) = -1;
    }

    U32_AT(GAME, 0) &= ~2U;
}

void Scene_LoadFieldBg(void) {
    int processed;
    int status;

    if (!(U32_AT(GAME, 0) & 1)) {
        return;
    }

restart_first:
    Scene_LoadFieldBgReadRange((u8 *)&D_800930EA_o, PTR_AT(GAME, 0x160));
    if (Scene_LoadFieldBgPollOrRetry() == -1) {
        goto restart_first;
    }

restart_second:
    processed = 0;
    Scene_LoadFieldBgReadRange((u8 *)&D_800930EC_o, PTR_AT(GAME, 0x174));
    status = 1;
    do {
        if (!processed) {
            Scene_LoadFieldBgLoadTimList(PTR_AT(GAME, 0x160));
            Scene_LoadFieldBgPackTextureDescriptors(0x20, 0x40);
            Scene_LoadFieldBgLoadImages(PTR_AT(GAME, 0x160));
            processed = 1;
        }
        if (status == -1) {
            goto restart_second;
        }
        status = CdRom_PollReady();
    } while (status != 0);

restart_third:
    processed = 0;
    Scene_LoadFieldBgReadRange((u8 *)&D_800930EE_o, PTR_AT(GAME, 0x180));
    status = 1;
    do {
        if (!processed) {
            Gpu_LoadTimImage(PTR_AT(GAME, 0x174));
            Scene_LoadFieldBgPackTextureDescriptors(0, 0x20);
            processed = 1;
        }
        if (status == -1) {
            goto restart_third;
        }
        status = CdRom_PollReady();
    } while (status != 0);

restart_fourth:
    processed = 0;
    Scene_LoadFieldBgReadRange((u8 *)&D_800930F0_o, PTR_AT(GAME, 0x14C));
    status = 1;
    do {
        if (!processed) {
            Gpu_LoadTimImage(PTR_AT(GAME, 0x180));
            Battle_DrawHPBar();
            processed = 1;
        }
        if (status == -1) {
            goto restart_fourth;
        }
        status = CdRom_PollReady();
    } while (status != 0);

restart_fifth:
    processed = 0;
    Scene_LoadFieldBgReadRange((u8 *)&D_800930E0_o, PTR_AT(GAME, 0x16C));
    status = 1;
    do {
        if (!processed) {
            Scene_LoadFieldBgResolveSubresources(PTR_AT(GAME, 0x14C));
            processed = 1;
        }
        if (status == -1) {
            goto restart_fifth;
        }
        status = CdRom_PollReady();
    } while (status != 0);

restart_sixth:
    processed = 0;
    Scene_LoadFieldBgReadRange((u8 *)&D_80093126_o, PTR_AT(GAME, 0x188));
    status = 1;
    do {
        if (!processed) {
            Scene_LoadFieldBgLoadTimList(PTR_AT(GAME, 0x16C));
            processed = 1;
        }
        if (status == -1) {
            goto restart_sixth;
        }
        status = CdRom_PollReady();
    } while (status != 0);

    Scene_LoadFieldBgLoadTimList(PTR_AT(GAME, 0x188));
    Scene_LoadFieldBgFinish();
}
