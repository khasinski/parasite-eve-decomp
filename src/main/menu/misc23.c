/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */
#include "include_asm.h"

typedef signed short s16;
typedef unsigned int u32;
typedef unsigned char u8;

typedef struct {
    int value;
    char pad[8];
} WideInt;

extern int g_BattleAttackAnimFrame;
extern u8 *g_ActiveActor[3];
extern u8 *g_PlayerEntity[3];
extern int g_ActiveDrawSlot[3];
extern WideInt g_RenderAnimVramOffset;
extern WideInt D_8009CDD8_load asm("g_RenderAnimVramOffset");
extern WideInt D_8009CDD8_update asm("g_RenderAnimVramOffset");
extern void *D_800B0DF8[3];
extern u8 *g_OtBufferTable[];
extern u8 *g_RenderAnimBufferBase[];

void Asset_Find08Alt(int arg0, int arg1, int arg2, int arg3, int arg4);
void Render_SetGteScreenOffset(void);
int Render_DecompressAnimFrame(void *arg0, void *arg1, int arg2, int arg3, void *arg4, void *arg5);
void Render_ResetGteScreenOffset(void);

typedef unsigned char u8_2;
typedef unsigned int u32_2;
typedef short s16_2;

extern u8_2 g_BattleSaveOverlayActive;
extern u8_2 g_MenuActiveMode;
extern u8_2 D_8009CE88;
extern u32_2 g_CurItemEffectData;

void Tbl_ResetAll(void);
int Save_GetMetadataWindowIndex(void);
void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3);
void Render_SetupColorTable(int arg0, int arg1, s16_2 *arg2);
void Draw_SetCursor(int arg0, int arg1);
void Draw_AllocColorGradient(int arg0, int arg1, int arg2, int arg3);

void Render_AnimationFrame(void) {
    int size;
    int width;
    register int frame asm("$4");
    u8 *ctx_a1;
    register u8 *ctx_a0 asm("$4");
    u8 *ctx;
    int check;
    int drawIndex;
    int delta;
    int temp;
    int framePad[3];
    register int code asm("$4");
    int zero_arg;
    int call_arg2;
    int call_arg3;
    int call_arg4;

    frame = g_BattleAttackAnimFrame;
    if ((u32) frame < 10U) {
        ctx_a1 = g_ActiveActor[0];
        if ((*(int *) (ctx_a1 + 0x4C) & 0x30) == 0x10) {
            temp = (frame << 2) + frame;
            size = temp << 18;
        } else {
            size = ((u32) ((*(s16 *) (*(u8 **) (ctx_a1 + 0x68) + 2) << 16) * frame)) / 10U;
        }
        check = g_BattleAttackAnimFrame;
        width = check << 6;
    } else {
        ctx_a0 = g_ActiveActor[0];
        size = 0xC80000;
        if ((*(int *) (ctx_a0 + 0x4C) & 0x30) != 0x10) {
            size = *(s16 *) (*(u8 **) (ctx_a0 + 0x68) + 2) << 16;
        }
        width = (g_BattleAttackAnimFrame << 4) + 10;
        asm volatile("" : : : "memory");
        check = g_BattleAttackAnimFrame;
    }

    code = 0x457;
    if (check == 0) {
        ctx = g_PlayerEntity[0];
        call_arg2 = *(s16 *) (ctx + 0x2A);
        call_arg3 = *(s16 *) (ctx + 0x2E);
        call_arg4 = *(s16 *) (ctx + 0x32);
        asm volatile("addu\t%0,$0,$0" : "=r"(zero_arg));
        Asset_Find08Alt(code, zero_arg, call_arg2, call_arg3, call_arg4);
    }

    Render_SetGteScreenOffset();

    drawIndex = g_ActiveDrawSlot[0];
    delta = Render_DecompressAnimFrame(D_800B0DF8[0], g_PlayerEntity[0] + 0x28, size, width & 0xFFFE,
                          g_OtBufferTable[drawIndex] + 8, g_RenderAnimBufferBase[drawIndex] + D_8009CDD8_load.value);
    g_RenderAnimVramOffset.value = D_8009CDD8_update.value + delta;

    Render_ResetGteScreenOffset();
    g_BattleAttackAnimFrame++;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/misc23", Save_DrawSlotMetadata);

void Menu_SaveOverlayDraw(void) {
    s16_2 value;
    int x;
    int y;
    u32_2 d1f8;

    switch (g_BattleSaveOverlayActive) {
    case 1:
        value = -1;
        Tbl_ResetAll();
        if (Save_GetMetadataWindowIndex() != 0) {
            x = 0x14;
        } else {
            x = 0x61;
        }
        if (g_MenuActiveMode < 2) {
            y = 0xF;
        } else {
            y = 0xC3;
        }
        Menu_SetTextCursorRect(x, y, 0, 0);
        Render_SetupColorTable(0, 2, &value);
        asm volatile(
            "lbu\t$4,g_BattleSaveOverlayActive\n"
            "li\t$2,2\n"
            "sb\t$2,g_TextboxEntries\n"
            "li\t$2,75\n"
            "sb\t$2,D_8009CE88"
            :
            :
            : "$1", "$2", "$4", "memory");
        d1f8 = g_CurItemEffectData;
        asm volatile(
            "li\t$3,0x02000000\n"
            "sw\t$2,g_SaveMetadataTextPtr\n"
            "lw\t$2,D_800BCEB4\n"
            "addiu\t$4,$4,1\n"
            "sb\t$4,g_BattleSaveOverlayActive\n"
            "or\t$2,$2,$3\n"
            "sw\t$2,D_800BCEB4"
            :
            : "r"(d1f8)
            : "$1", "$3", "$4", "memory");
        break;
    case 2:
        if (D_8009CE88 == 0) {
            Tbl_ResetAll();
            g_BattleSaveOverlayActive = 0;
        }
        break;
    }

    D_8009CE88--;
    Draw_SetCursor(0, g_MenuActiveMode < 2 ? 0xB : 0xBF);
    Draw_AllocColorGradient(0x140, 0x14, 0, 0);
}
