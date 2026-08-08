#include "common.h"
extern char *D_80011610;
extern u16 D_80093166;
extern u16 D_80093168;
extern s32 g_ActiveDrawSlot __asm__("D_8009CDDC");
extern char *D_80125B88;
extern char *D_80125B90;
extern char *D_80125B98;
extern char *D_80125BA0;
extern s32 D_80172C90;
extern s32 D_80172C94;
extern s32 D_80172C98;
extern s32 D_80172C9C;
extern s16 g_BootDisplayTPage;
extern s16 g_BootDisplayClut;
extern s32 g_BootDisplayTextScrollX;
extern s32 g_BootDisplayTextScrollStepX;
extern volatile s16 g_BootDisplayStateTimer;
extern s16 g_BootDisplayTransferProgress;
extern s32 g_BootDisplayBufferIndex;
extern s32 g_BootDisplayState;
extern char g_RenderDisplayEnvArray[] __asm__("D_800BCE80");
extern char D_8012682C[];

extern void func_80074A44(s32 arg0);
extern void func_80074BB8(s32 arg0);
extern void func_80074D28(s32 arg0);
extern void func_80074DC0(s32 arg0);
extern void func_80074F44(void *rect, s32 arg1, s32 arg2, s32 arg3);
extern void func_800755F0(void *entry);
extern s32 func_80071944(void *arg0);
extern s32 func_80071964(void *arg0);
extern s32 func_80071994(void *arg0);
extern s32 func_800719C4(void *arg0);
extern s32 func_80077A64(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern s32 func_80077AA4(s32 arg0, s32 arg1);
extern void func_8010BD4C(void *arg0);
extern void func_8010BE3C(s32 arg0);
extern void func_80124F40(s32 arg0);

void func_8012479C(void) {
    s16 rect[4];
    u16 line;
    s32 delta;
    char *base;
    char *start;
    char *end;
    s32 offset;
    u16 *screenY;
    void *resource;

    func_80074A44(1);
    func_80074BB8(0);
    func_80074D28(0);

    screenY = &D_80093168;
    g_BootDisplayTextScrollX = 0xE000;
    g_BootDisplayTextScrollStepX = -0x100;
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x3FF;
    rect[3] = 0x1FF;
    g_BootDisplayBufferIndex = 0;
    g_BootDisplayState = 0;
    g_BootDisplayTransferProgress = 0;
    g_BootDisplayStateTimer = 0;
    g_BootDisplayStateTimer = 0;

    line = *screenY;
    base = D_80011610;
    delta = ((s32)line - D_80093166) << 11;
    start = base + delta;
    delta += 0x54000;
    end = base + delta;
    D_80125B88 = start;
    D_80125B90 = end;
    D_80125B98 = end;
    D_80125BA0 = start;
    func_80074F44(rect, 0, 0, 0);
    func_80074DC0(0);
    func_8010BE3C(0);
    offset = 0x43000;
    func_8010BD4C(D_80011610 + (((s32)*screenY - D_80093166) << 11) + offset);

    resource = D_8012682C;
    D_80172C98 = func_80071994(resource);
    D_80172C9C = func_800719C4(resource);
    D_80172C90 = func_80071964(resource);
    D_80172C94 = func_80071944(resource);
    g_BootDisplayTPage = func_80077A64(0, 0, ((s16 *)D_80172C90)[0], ((s16 *)D_80172C90)[1]);
    g_BootDisplayClut = func_80077AA4(((s16 *)D_80172C94)[0], ((s16 *)D_80172C94)[1]);
    func_80124F40(0);
    func_80124F40(1);
    func_800755F0(g_RenderDisplayEnvArray + (g_ActiveDrawSlot * 20));
    func_80074D28(1);
}
