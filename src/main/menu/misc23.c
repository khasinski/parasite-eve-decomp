/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
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

extern struct { char _[16]; } D_8009D1A8_o __asm__("D_8009D1A8");
extern struct { char _[16]; } D_8009D1AC_o __asm__("D_8009D1AC");
extern struct { char _[16]; } g_SaveMetadataTextPtr_o __asm__("g_SaveMetadataTextPtr");
extern struct { char _[16]; } D_800BCEB4_o __asm__("D_800BCEB4");
extern u8_2 g_BattleSaveOverlayActive;
extern u8_2 g_MenuActiveMode;
extern u8_2 D_8009CE88;
extern u32_2 g_CurItemEffectData;
extern u8_2 g_TextboxEntries[];
extern u8_2 D_80091464[];
extern u8_2 D_80091474[];
extern u8_2 D_80091480[];
extern u8_2 D_800914AC[];
extern u8_2 D_800914D4[];
extern u8_2 D_800914FC[];
extern u8_2 D_80091520[];
extern u8_2 D_80091544[];
extern u8_2 D_80091570[];

#define D_8009D1A8 (*(void **)&D_8009D1A8_o)
#define D_8009D1AC (*(u32_2 *)&D_8009D1AC_o)
#define D_8009D1AC_BYTE (*(u8_2 *)&D_8009D1AC_o)
#define g_SaveMetadataTextPtr (*(u8_2 **)&g_SaveMetadataTextPtr_o)
#define D_800BCEB4 (*(u32_2 *)&D_800BCEB4_o)

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

void Save_DrawSlotMetadata(void) {
    void *overlay;
    void *entry;
    u32_2 state;
    int phase;
    int prompt;
    s16_2 colors[3];

    overlay = D_8009D1A8;
    if (overlay == 0 || *(void **)overlay == 0 || *(int *)(*(u8_2 **)overlay + 0x10) <= 0) {
        Tbl_ResetAll();
        D_8009D1AC &= ~0x300;
    }

    state = D_8009D1AC;
    phase = (state >> 8) & 3;

    if (phase == 1) {
        entry = *(void **)D_8009D1A8;
        colors[0] = *(int *)((u8_2 *)entry + 0x10);
        colors[1] = *(int *)((u8_2 *)entry + 0x88);
        colors[2] = 0;

        Tbl_ResetAll();
        Menu_SetTextCursorRect(Save_GetMetadataWindowIndex() == 0 ? 0x61 : 0x14,
                               g_MenuActiveMode < 2 ? 0xF : 0xC3, 0, 0);
        Render_SetupColorTable(0, 2, colors);

        prompt = (D_8009D1AC >> 10) & 3;
        if (prompt == 0) {
            g_SaveMetadataTextPtr = D_80091464;
            g_TextboxEntries[0] = 2;
        } else if (prompt == phase) {
            g_SaveMetadataTextPtr = D_80091474;
            g_TextboxEntries[0] = 2;
        } else {
            g_TextboxEntries[0] = 2;
        }

        state = D_8009D1AC;
        D_8009D1AC = (state & ~0x300) | ((((state >> 8) & 3) + 1 & 3) << 8);
        D_800BCEB4 |= 0x02000000;
    } else if (phase == 2) {
        if (D_8009D1AC_BYTE == 0) {
            D_8009D1AC = (state & ~0x300) | 0x200;
            D_8009D1AC_BYTE = 0x4B;

            state = D_8009D1AC;
            if (state & 0x1000) {
                g_SaveMetadataTextPtr = D_80091480 + Save_GetMetadataWindowIndex() * 22;
                D_8009D1AC &= ~0x1000;
            } else if (state & 0x2000) {
                g_SaveMetadataTextPtr = D_800914AC + Save_GetMetadataWindowIndex() * 20;
                D_8009D1AC &= ~0x2000;
            } else if (state & 0x4000) {
                g_SaveMetadataTextPtr = D_800914D4 + Save_GetMetadataWindowIndex() * 20;
                D_8009D1AC &= ~0x4000;
            } else if (state & 0x8000) {
                g_SaveMetadataTextPtr = D_800914FC + Save_GetMetadataWindowIndex() * 18;
                D_8009D1AC &= ~0x8000;
            } else if (state & 0x10000) {
                g_SaveMetadataTextPtr = D_80091520 + Save_GetMetadataWindowIndex() * 18;
                D_8009D1AC &= ~0x10000;
            } else if (state & 0x20000) {
                g_SaveMetadataTextPtr = D_80091544 + Save_GetMetadataWindowIndex() * 22;
                D_8009D1AC &= ~0x20000;
            } else if (state & 0x40000) {
                g_SaveMetadataTextPtr = D_80091570 + Save_GetMetadataWindowIndex() * 21;
                D_8009D1AC &= ~0x40000;
            } else {
                Tbl_ResetAll();
                D_8009D1AC &= ~0x300;
            }
        }
    }

    D_8009D1AC_BYTE--;
    Draw_SetCursor(0, g_MenuActiveMode < 2 ? 0xB : 0xBF);
    Draw_AllocColorGradient(0x140, 0x14, 0, 0);
}

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
