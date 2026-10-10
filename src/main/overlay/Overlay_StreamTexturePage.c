#include "common.h"
#include "pe1/game_state.h"
#include "pe1/cdrom.h"
#include "pe1/asset_tim.h"
s32 DrawSync(s32 arg0);
extern s8 D_800B0CE0;
extern s8 g_LoadedTexturePageId;
extern u16 g_EntityTexLbaStartTbl[];
s32 Overlay_StreamTexturePage(void)
{
  Pe1GameState *state;
  register u16 *ranges;
  s32 retry;
  u16 *next_ranges;
  s32 offset;
  u16 start;
  s32 poll;
  state = &g_GameState;
  if (D_800B0CE0 != g_LoadedTexturePageId)
  {
    DrawSync(0);
    restart:
    ranges = g_EntityTexLbaStartTbl;

    next_ranges = ranges + 1;
    retry = -1;
    do
    {
      offset = ((s8)state->room_type + 0x2B) * 2;
      start = *((u16 *) (offset + ((s32) ranges)));
    }
    while (CdRom_ReadSectorsFromLba(state->pe_image_base_lba + start, state->scene_load_scratch, (*((u16 *) (offset + ((s32) next_ranges)))) - start) == retry);
    ;
    do
    {
      poll = CdRom_PollReady();
      if (poll == 0)
      {
        break;
      }
      if (poll == ((s32) ((u16 *) (-1))))
      {
        goto restart;
      }
    }
    while (1);
    Asset_LoadTimImage((TimFile *)state->scene_load_scratch);
    state->field_bg_cache_bank = (s8)state->room_type;
  }
  return 0;
}

/* Disc-change gating for scene-area transitions. */
extern unsigned char g_SceneAreaType;
extern unsigned char g_SavedSceneAreaType;
extern unsigned char g_DiscChangeFlags;

int CdRom_DetectDiscChange(void) {
    unsigned int *state = &g_GameState.flags;
    int offset = g_SceneAreaType - 0xA;

    if ((unsigned int)offset < 5) {
        int value = g_SavedSceneAreaType;

        if (g_SceneAreaType != (unsigned char)value) {
            *state |= 0x200000;
        }

        if (((unsigned int)offset >> 1) != ((value - 0xA) / 2)) {
            g_DiscChangeFlags |= 4;
        }
    }

    return 0;
}
