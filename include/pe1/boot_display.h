#ifndef PE1_BOOT_DISPLAY_H
#define PE1_BOOT_DISPLAY_H

#include "common.h"

typedef struct {
    s32 tag;
    s32 code;
} BootDisplayDrawModePrim;

typedef struct {
    s32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    u16 x0;
    u16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    u16 w;
    u16 h;
} BootDisplaySpritePrim;

typedef struct {
    BootDisplayDrawModePrim draw_mode;
    BootDisplaySpritePrim sprite;
} BootDisplaySpritePacket;

/* One 16-byte record per display buffer: GPU position, pixels, and decode target. */
typedef struct {
    u16 x, y;
    char *pixels;
    u16 transferX, transferY;
    char *decodeTarget;
} BootDisplayImageBuffer;
extern BootDisplayImageBuffer g_BootDisplayImageBuffers[2] __asm__("D_80125B84");

typedef struct {
    void (*init)(void);
    void (*step)(void);
    s32 (*text)(void);
    void (*glyph)(s16, s16, u8);
    void (*transition)(u8);
    void (*present)(void);
    void (*shutdown)(void);
} BootDisplayCallbacks;
extern BootDisplayCallbacks *g_BootDisplayCallbacks;
extern u16 g_BootDisplayFontStyle;
extern const u8 *D_80125C88[];
extern const u8 **g_BootDisplayTextCursor;

extern char *g_BootDisplayOrderingTable;
extern u16 g_BootDisplayTPage;
extern u16 g_BootDisplayClut;
extern char *g_BootDisplayPrimitiveCursor;
extern s32 g_BootDisplayTextScrollX;
extern s32 g_BootDisplayTextScrollStepX;
extern s32 g_BootDisplayTransition;
extern s32 g_BootDisplayBufferIndex;
extern volatile s16 g_BootDisplayStateTimer;
extern s16 g_BootDisplayTransferProgress;
extern s32 g_BootDisplayState;
extern s16 *D_80172C90;
extern s16 *D_80172C94;
extern s32 D_80172C98;
extern s32 D_80172C9C;
extern u16 D_80093166;
extern u16 D_80093168;
extern char *D_80011610;
extern char D_8012682C[];
/* Two buffer records, each holding four image addresses. */
extern char *D_80125B88[8];
extern char *D_80125B90;
extern char *D_80125B98;
extern char *D_80125BA0;
extern volatile s32 D_80172CF0;
extern volatile s32 D_80172CEC;
extern s32 D_80172CE8;
extern s32 D_80172CE4;
extern u8 D_80125AF0[];
extern u16 D_800B0DD4;
extern volatile u16 D_80172CF8;
extern volatile u16 D_80172CFA;
extern char *D_80172D00;
extern volatile s32 D_80172CF4;
extern char *D_80125B48;
extern s32 g_ActiveDrawSlot __asm__("D_8009CDDC");
extern char g_RenderDisplayEnvArray[] __asm__("D_800BCE80");
extern char g_RenderDrawEnvArray[] __asm__("D_800BCDC8");

void func_80077AC4(void *ordering_table, void *primitive);
void func_80077B04(void *primitive, s32 code);
void func_80077C84(void *primitive, s32 x, s32 y, u16 tpage);
void func_80077CB4(void *primitive, void *next);
void func_80074A44(s32 value);
s32 func_80073A44(s32 value);
void func_800870F0(s32 value);
void func_801258EC(s32, s32);
s32 func_8007EE84(s32, void *, void *, s32);
s32 func_8007F418(s32, void *);
void func_800824C8(s32 (*)(u8));
void func_80073D24(void (*)(void));
s32 Boot_StartPlayback(void);
void Boot_CheckPlaybackTime(void);
s32 func_8007F72C(void);
s32 func_8007F7A8(void);
/* PsyQ exit: the failure path never returns to the transfer callback. */
void func_800719E4(s32) __attribute__((noreturn));
void func_80080AE4(void *, s32);
s32 Boot_StepPlaybackTransfer(u8 mode);
void func_8007EC14(void);
void func_80080CC8(void *);
s32 func_8007F778(void);
s32 Boot_InitPlaybackFile(void);
void func_80074BB8(s32 value);
void func_80074D28(s32 value);
void func_80074DC0(s32 value);
void func_80074F44(void *rect, s32 x, s32 y, s32 mode);
s32 func_800718D0(void *resource);
s32 func_800719C4(void *resource);
s32 func_80071994(void *resource);
s16 *func_80071964(void *resource);
s16 *func_80071944(void *resource);
s32 func_80077A64(s32, s32, s32, s32);
s32 func_80077AA4(s32, s32);
void func_8010BE3C(s32 value);
void func_8010BD4C(void *address);
void func_800755F0(void *entry);
void func_8007506C(void *rect, void *image);
void func_80075424(void *draw_env);
void Boot_PresentFrame(void);
void func_800754E4(void *dst, void *src);
void func_80124F40(s32 value);

/* Callable entry points inside neighboring boot-display code. */
s32 Boot_GetGlyphIndex(u8 character);
void func_8012462C(void *primitive, u16 x, u16 y, s32 u, s32 v);

#endif
