/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

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
