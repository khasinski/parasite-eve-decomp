#ifndef MENU_MEMCARD_MENU_H
#define MENU_MEMCARD_MENU_H

#include "menu_memcard_screen.h"
#include "menu_memcard_image.h"

/* Matching views of the existing two pointer slots, not extra storage.
 * Separate names retain retail's reloads instead of caching their addresses. */
extern struct { MemcardScreenBuffer *value; } secondScreen asm("D_801D11C0");
extern MemcardScreenBuffer *D_801D11C0;
extern MemcardScreenBuffer *firstDrawScreen asm("D_801D11BC");

extern DISPENV D_800BCE80, D_800BCE94, D_801D1550, D_801D1564;
extern DRAWENV D_800BCDC8, D_800BCE24, D_801D1498, D_801D14F4;
extern s16 D_800BCE8A, D_800BCE9E;
extern u8 D_800B0DCD;
extern s32 D_8009D1BC, D_8009CDDC;
extern u8 *D_80011610, *D_800B0E38, *D_800B0E3C, *D_800B0E50, *D_800B0E54;
extern u8 D_8019319C[];

void func_80074924(DRAWENV *, int, int, int, int); /* SetDefDrawEnv */
void func_800749D8(DISPENV *, int, int, int, int); /* SetDefDispEnv */
void func_80074F44(RECT *, int, int, int); /* ClearImage */
void func_8007512C(RECT *, int, int); /* MoveImage */
void func_80190660(void);
void Draw_InitBuffers(void);
void func_80036DF8(void);
void func_80036E34(void);
void func_8003EB04(void);
void func_8003FFAC(int);
void func_80042538(void);
void func_800425DC(void);
void func_8004D084(int);
void func_8005C1EC(int);
int func_8005C498(int);
void func_8005E57C(int);
void func_8005E6E4(void *);
void func_8005E6F0(void);
void func_8005E788(int);
void func_8018F2F4(void);
void func_8018F468(void);
MemcardImageNode *func_8018FBC0(int);
void func_80190064(void);
int func_80192CE8(int);

#endif
