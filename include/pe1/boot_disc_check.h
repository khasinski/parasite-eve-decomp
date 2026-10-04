#ifndef PE1_BOOT_DISC_CHECK_H
#define PE1_BOOT_DISC_CHECK_H

#include "common.h"
#include "pe1/game_state.h"
#include "pe1/psyq_gpu.h"
#include "pe1/cdrom.h"
#include "pe1/textbox.h"

/*
 * Render_InitDisplayLists: boot notice screen. Streams the notice TIM and
 * its fog layer from PE.IMG, then runs a ten-state disc check (DsCommand
 * read, DsSync, shell status, PE.IMG mount) under the double-buffered
 * display until the expected disc is in the drive.
 */

extern u16 D_800930D8[];
extern s32 D_8009CDDC;
/* Ordering table of each draw slot (Boot_InitMemoryLayout). */
extern u32 *D_800B0E38[2];
extern DISPENV D_800BCE80[2];
extern DRAWENV D_800BCDC8[2];
extern u8 D_800B0DCD;
extern u16 D_800B0DD4;

int VSync(int mode);
void SetDispMask(int mask);
void DrawSync(int mode);
void ClearImage(RECT *rect, int r, int g, int b);
void MoveImage(RECT *rect, int x, int y);
void ClearOTagR(u32 *table, int length);
void DrawOTag(u32 *table);
DRAWENV *PutDrawEnv(DRAWENV *env);
int *Gpu_LoadTimImage(void *tim);
void Boot_BuildRenderFlagTable(void);
void Render_SetupFogLayer(void *source);
void Render_InitEntityPool(int mode);
int Render_AllocParticleNode(int command, void *parameter, void *callback,
                             int count);
int Render_FindParticleEffect(int id, void *result);
int CdRom_GetCmdStatus(void);
int OpenPeImage(void);
int Render_InitDisplayLists(int mode);

#endif
