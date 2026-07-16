/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

extern struct { char _[16]; } D_800B0CD8_o __asm__("D_800B0CD8");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D2E8_o __asm__("D_8009D2E8");
extern struct { char _[16]; } D_800B0D10_r __asm__("D_800B0D10");
extern struct { char _[16]; } D_800B0D10_w __asm__("D_800B0D10");
extern struct { char _[16]; } D_800B0D14_o __asm__("D_800B0D14");
extern struct { char _[16]; } D_800B0D16_o __asm__("D_800B0D16");
extern struct { char _[16]; } D_800B89F8_o __asm__("D_800B89F8");

#define D_800B0CD8 (*(u32 *)&D_800B0CD8_o)
#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D1A0 (*(u32 *)&D_8009D1A0_o)
#define D_8009D2E8 (*(u32 *)&D_8009D2E8_o)
#define D_800B0D10_READ (*(u8 **)&D_800B0D10_r)
#define D_800B0D10_WRITE (*(u8 **)&D_800B0D10_w)
#define D_800B0D14 (*(u16 *)&D_800B0D14_o)
#define D_800B0D16 (*(u16 *)&D_800B0D16_o)
#define D_800B0CEC ((u8 *)&D_800B0CD8_o + 0x14)
#define D_800B89F8 ((u32 *)&D_800B89F8_o)

void Render_SetGteScreenOffset(void);
void Render_TransformVertices(u8 *actor);
void Render_TransformMorphVertices(u8 *actor, u32 *view_matrix);
void Render_DrawEntity(u8 *actor, u32 *view_matrix);
void Render_ResetGteScreenOffset(void);

int Scene_UpdateBgDraw(void) {
    u8 *entity;
    register u8 *gameState asm("$16") = (u8 *)&D_800B0CD8_o;

    if (*(u32 *)gameState & 0xC0000) {
        return 0;
    }

    entity = D_8009D254;
    if (entity == 0) {
        return 0;
    }

    if (*(u32 *)(entity + 0x98) & 0x20000040) {
        return 0;
    }

    if (!(D_8009D1A0 & 2) && (D_8009D2E8 & 2)) {
        return 0;
    }

    if (D_800B0D10_READ == 0) {
        register u8 *bgInit asm("$2");

        bgInit = entity + 0x1B4;
        D_800B0D10_WRITE = bgInit;
        D_800B0D14 = 3;
        D_800B0D16 = 0x12;
    }

    gameState += 0x14;
    Render_SetGteScreenOffset();
    Render_TransformVertices(gameState);
    Render_TransformMorphVertices(gameState, D_800B89F8);

    if ((D_8009D1A0 & 2) || D_8009D254[0x252] != 0) {
        Render_DrawEntity(gameState, D_800B89F8);
    }

    Render_ResetGteScreenOffset();
    return 0;
}
