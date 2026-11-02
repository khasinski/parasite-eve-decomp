#ifndef PE1_BOOT_DISC_CHECK_H
#define PE1_BOOT_DISC_CHECK_H

#include "common.h"
#include "pe1/cdrom.h"
#include "pe1/display_env.h"
#include "pe1/game_state.h"
#include "pe1/gpu_image.h"
#include "pe1/psyq_ds_queue.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_packets.h"
#include "pe1/textbox.h"

/*
 * Render_InitDisplayLists: boot notice screen. Streams the notice TIM and
 * its fog layer from PE.IMG, then runs a ten-state disc check (DsCommand
 * read, DS_system_status, shell status, PE.IMG mount) under the double-buffered
 * display until the expected disc is in the drive.
 */

extern u16 g_PeImageSectorTable[];
extern s32 g_ActiveDrawSlot;
/* Ordering table of each draw slot (Boot_InitMemoryLayout). */
extern RenderBufferPrefix D_800B0E38;
extern RenderBufferPrefix D_800B08F8;

void Boot_BuildRenderFlagTable(void);
void Render_SetupFogLayer(void *source);
void ResetGraph(int mode);
int DsStatus(void);
int Asset_OpenPeImage(void);
int Render_InitDisplayLists(int mode);

#endif
