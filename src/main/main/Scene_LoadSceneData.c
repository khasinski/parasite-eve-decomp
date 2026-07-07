/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)

typedef struct { char b[16]; } __attribute__((aligned(1), packed)) Copy16u;
typedef struct { u32 w[4]; } Copy16a;

typedef struct { s16 x, y, w, h; } RECT;

int ClearImage(RECT *rect, u8 r, u8 g, u8 b);
s32 CdRom_ReadSectorsFromLba(s32, s32, s32);
int CdRom_PollReady(void);
void Menu_FullInit(void);
int Asset_FindTable08ByU32Key(void *arg0, s32 arg1);
void Spu_UploadSampleBlockBlocking(int arg0, int arg1);

extern struct { char _[16]; } g_PeImageBaseLba_o __asm__("g_PeImageBaseLba");
extern struct { char _[16]; } D_800930DC_o __asm__("D_800930DC");
extern struct { char _[16]; } D_800930DE_o __asm__("D_800930DE");
extern struct { char _[16]; } D_800930E4_o __asm__("D_800930E4");
extern struct { char _[16]; } D_800930E6_o __asm__("D_800930E6");
extern struct { char _[16]; } D_800A8028_o __asm__("g_GlyphMetricsTable");
extern struct { char _[16]; } D_800E2858_o __asm__("D_800E2858");
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
extern struct { char _[16]; } D_800B0E6C_o __asm__("g_SceneLoadScratchBuffer");

void Scene_LoadSceneData(void) {
    RECT rect;
    u8 *gb;
    s32 lba;
    u8 *p0;
    s32 t;
    s32 flag;
    u8 *src;
    u8 *dst;
    u8 *end;

    rect.w = 0x3FF;
    rect.x = 0;
    rect.y = 0;
    rect.h = 0x1FF;
    gb = (u8 *)&D_800B0CD8_o;
    lba = *(s32 *)&g_PeImageBaseLba_o;
    ClearImage(&rect, 0, 0, 1);

restart1:
    p0 = (u8 *)&D_800930DC_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, (s32)&D_800A8028_o, *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (t == -1) {
            goto restart1;
        }
        t = CdRom_PollReady();
    } while (t != 0);

    flag = 0;
restart2:
    p0 = (u8 *)&D_800930DE_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, *(s32 *)(gb + 0x194), *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (flag == 0) {
            Menu_FullInit();
            flag = 1;
        }
        if (t == -1) {
            goto restart2;
        }
        t = CdRom_PollReady();
    } while (t != 0);

    src = *(u8 **)(gb + 0x194);
    dst = (u8 *)&D_800E2858_o;
    if (((u32)src | (u32)dst) & 3) {
        register u8 *e1 asm("$8") = src + 0x10A50;
        do {
            *(Copy16u *)dst = *(Copy16u *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != e1);
    } else {
        register u8 *e2 asm("$8") = src + 0x10A50;
        do {
            *(Copy16a *)dst = *(Copy16a *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != e2);
    }

    *(s32 *)(gb + 0x148) = (s32)&D_800E2858_o;
    *(s32 *)(gb + 0x140) = Asset_FindTable08ByU32Key((s32)&D_800E2858_o, 0x57D40D84);
    *(s32 *)(gb + 0x144) = Asset_FindTable08ByU32Key(*(s32 *)(gb + 0x148), 0x57D41D84);

restart3:
    p0 = (u8 *)&D_800930E4_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, *(s32 *)(gb + 0x194), *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (t == -1) {
            goto restart3;
        }
        t = CdRom_PollReady() != 0;
    } while (t != 0);

    Spu_UploadSampleBlockBlocking(*(s32 *)&D_800B0E6C_o, 1);

restart4:
    p0 = (u8 *)&D_800930E6_o;
    while (CdRom_ReadSectorsFromLba(lba + *(u16 *)p0, *(s32 *)(gb + 0x194), *(u16 *)(p0 + 2) - *(u16 *)p0) == -1) {}
    t = 1;
    do {
        if (t == -1) {
            goto restart4;
        }
    } while ((t = (CdRom_PollReady() != 0)) != 0);

    dst = *(u8 **)(gb + 0x130);
    __asm__ __volatile__("" : "=r"(dst) : "0"(dst));
    src = *(u8 **)(gb + 0x194);
    end = src + 0x1400;
    if (((u32)src | (u32)dst) & 3) {
        do {
            *(Copy16u *)dst = *(Copy16u *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != end);
    } else {
        do {
            *(Copy16a *)dst = *(Copy16a *)src;
            src += 0x10;
            dst += 0x10;
        } while (src != end);
    }
}
