/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Entity_SetActionMode(void *, M2C_UNK);      /* extern */
M2C_UNK Render_SetupEntityPrims(void *, s32, s32, M2C_UNK, s32, s32, s32, s32, M2C_UNK *, s32); /* extern */
M2C_UNK Render_DrawWithAnim(void *, s32, M2C_UNK, M2C_UNK *, M2C_UNK *); /* extern */
M2C_UNK Render_InitRoomPrimState(void *);                      /* extern */
s32 Scene_LoadEntityTexture();                                /* extern */
M2C_UNK Scene_SetDiscSide(s8);                          /* extern */
s32 Scene_LoadEntityTextures();                                /* extern */
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(void **)&g_PlayerEntity_o)
extern struct { char _[16]; } g_PlayerEntity_s1 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s2 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s3 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s4 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s5 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s6 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s7 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s8 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s9 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s10 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_s11 __asm__("g_PlayerEntity");
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
extern struct { char _[16]; } D_800B0CD8_b __asm__("g_GameState");
#define g_GameState (*(M2C_UNK *)&D_800B0CD8_o)
extern struct { char _[16]; } D_800B0CE2_o __asm__("g_SceneAreaType");
extern struct { char _[16]; } D_800B0CE2_w __asm__("g_SceneAreaType");
#define g_SceneAreaType (*(u8 *)&D_800B0CE2_o)
extern struct { char _[16]; } D_800B0CE3_o __asm__("g_SavedSceneAreaType");
extern struct { char _[16]; } D_800B0CE3_s1 __asm__("g_SavedSceneAreaType");
#define g_SavedSceneAreaType (*(u8 *)&D_800B0CE3_o)
extern struct { char _[16]; } D_800B0CE4_o __asm__("D_800B0CE4");
#define D_800B0CE4 (*(s8 *)&D_800B0CE4_o)
extern struct { char _[16]; } D_800B0CE6_o __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_w __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_o2 __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_w2 __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CD8_c __asm__("g_GameState");
extern struct { char _[16]; } D_800B0CD8_cw __asm__("g_GameState");
extern struct { char _[16]; } D_800B0CD8_d __asm__("g_GameState");
extern struct { char _[16]; } D_800B0CD8_dw __asm__("g_GameState");
#define g_DiscChangeFlags (*(u8 *)&D_800B0CE6_o)
extern struct { char _[16]; } D_800B0CEB_o __asm__("g_SceneAreaTypeDiscSwapBackup");
#define g_SceneAreaTypeDiscSwapBackup (*(u8 *)&D_800B0CEB_o)
extern struct { char _[16]; } D_800B0DC5_o __asm__("D_800B0DC5");
#define D_800B0DC5 (*(u8 *)&D_800B0DC5_o)
extern struct { char _[16]; } D_800B0E70_o __asm__("g_SceneMapPrimBaseTable");
#define g_SceneMapPrimBaseTable (*(s32 *)&D_800B0E70_o)
extern struct { char _[16]; } D_800B0EEC_o __asm__("D_800B0EEC");
#define D_800B0EEC (*(s32 *)&D_800B0EEC_o)
extern struct { char _[16]; } D_800B89F8_o __asm__("g_EntityRenderScratch");
#define g_EntityRenderScratch (*(M2C_UNK *)&D_800B89F8_o)
extern struct { char _[16]; } D_800BEA40_o __asm__("D_800BEA40");
#define D_800BEA40 (*(M2C_UNK *)&D_800BEA40_o)

s32 Scene_InitEntityPlayer(s32 arg0) {
    u8 *p0cd8;
    register s32 arg0v asm("$17");
    M2C_UNK sp28;
    s32 var_v0_2;
    register s32 tld2 asm("$2");
    register s8 var_v0 asm("$2");
    u32 temp_a1;
    u8 temp_v1;
    u8 tce6;
    u8 tce3;
    void *p39a;
    void *p39b;
    void *p39c;
    void *p39d;
    void *p39e;
    u32 th1;
    register u8 k0e asm("$2");
    register u8 ttb asm("$2");

    arg0v = arg0;
    p0cd8 = (u8 *)&D_800B0CD8_o;
    switch (D_800B0DC5) {
    case 32:
        (*(s32 *)&D_800B0CD8_b) = (*(s32 *)&D_800B0CD8_b) | 0x20000;
        if (arg0v != 0) {
            temp_v1 = g_SceneAreaType;
            k0e = 0xE;
            (*(u8 *)&D_800B0CE2_w) = k0e;
            g_SceneAreaTypeDiscSwapBackup = temp_v1;
            var_v0 = 0x21;
            goto block_20;
        } else {
            ttb = g_SceneAreaTypeDiscSwapBackup;
            g_SceneAreaType = ttb;
            var_v0 = 0x21;
            goto block_20;
        }
    case 33:
        var_v0 = 0x22;
        goto block_20;
    case 34:
        var_v0 = 0x23;
        goto block_20;
    case 35:
        temp_a1 = g_SceneAreaType - 0xA;
        if (temp_a1 < 5U) {
            tce3 = g_SavedSceneAreaType;
            if (g_SceneAreaType != tce3) {
                (*(s32 *)&D_800B0CD8_cw) = (s32) ((*(s32 *)&D_800B0CD8_c) | 0x200000);
            }
            th1 = temp_a1 >> 1;
            if (th1 != ((s32) (tce3 - 0xA) / 2)) {
                tce6 = g_DiscChangeFlags;
                (*(u8 *)&D_800B0CE6_w) = tce6 | 4;
            }
        }
        __asm__ __volatile__("");
        var_v0 = 0x24;
        goto block_20;
    case 36:
        if (Scene_LoadEntityTexture() != 1) {
            if (arg0v == 0) {
                var_v0 = 0x25;
                goto block_20;
            }
            goto block_19;
        }
        goto block_21;
    case 37:
        g_DiscChangeFlags |= 4;
        Scene_SetDiscSide(D_800B0CE4);
        M2C_FIELD(p0cd8, s8 *, 0xED) = 0x26;
        return 1;
    case 38:
        var_v0 = 0x27;
        if (Scene_LoadEntityTextures() == 1) {
            if ((u8) M2C_FIELD(p0cd8, u8 *, 0xEE) >= 0xBU) {
                goto block_19;
            }
            goto block_21;
        } else {
            var_v0 = 0x27;
            goto block_20;
        }
block_19:
        var_v0 = 0x27;
block_20:
        M2C_FIELD(p0cd8, s8 *, 0xED) = var_v0;
block_21:
        return 1;
    case 39:
        p39a = g_PlayerEntity;
        M2C_FIELD(p39a, s32 *, 0x1AC) = (s32) g_SceneMapPrimBaseTable;
        M2C_FIELD(p39a, s32 *, 0x1B0) = (s32) D_800B0EEC;
        Entity_SetActionMode(p39a, 0x15);
        p39b = (*(void **)&g_PlayerEntity_s2);
        Render_SetupEntityPrims(p39b + 0x1B4, M2C_FIELD(p39b, s32 *, 0x1AC), M2C_FIELD(p39b, s32 *, 0x278) + 0x50, 0x3C0, 0x100, 0, 0x1C0, 2, &sp28, 1);
        p39c = (*(void **)&g_PlayerEntity_s3);
        Render_InitRoomPrimState(p39c + 0x1B4);
        p39d = (*(void **)&g_PlayerEntity_s4);
        Render_DrawWithAnim(p39d + 0x1B4, M2C_FIELD(p39d, s32 *, 0x1B0), 0, &D_800BEA40, &g_EntityRenderScratch);
        p39e = (*(void **)&g_PlayerEntity_s5);
        M2C_FIELD(M2C_FIELD(p39e, void **, 0x1B4), s16 *, 0x14) = (s16) (M2C_FIELD(p39e, s16 *, 0x224) * 2);
        (*(s32 *)&D_800B0CD8_dw) = (s32) ((*(s32 *)&D_800B0CD8_d) & 0xFFF9FFFF);
        if (arg0v != 0) {
            tld2 = M2C_FIELD(p0cd8, s32 *, 0);
            var_v0_2 = tld2 | 0x80000;
        } else {
            var_v0_2 = M2C_FIELD(p0cd8, s32 *, 0) & 0xFFF7FFFF;
        }
        M2C_FIELD(p0cd8, s32 *, 0) = var_v0_2;
        M2C_FIELD(p0cd8, s8 *, 0xED) = 0x20;
        /* fallthrough */
    default:
        return 0;
    }
}
