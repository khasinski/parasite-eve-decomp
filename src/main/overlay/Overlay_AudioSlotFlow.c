#include "common.h"
#include "pe1/game_state.h"

/* The raw views retain retail addressing while deriving the key-table offset
 * from the shared Pe1GameState scene-audio layout. */
int Overlay_RegisterAudioSlot(int arg0, int arg1, int arg2, int arg3) {
    char *base;
    char *ptr;

    base = (char *)&g_GameState;
    ptr = &base[arg0 * 2];
    ptr[PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.keys)] = (char) arg1;
    ptr[PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.keys) + 1] = (char) arg2;
    base[arg0 + 0xFE] = (char) arg3;

    if (arg0 == 0) {
        *(int *) base |= 0x40;
    } else if (arg0 == 1) {
        *(int *) base |= 0x80;
    }

    return 0;
}

int Overlay_GetAudioSlotByKey(int arg0)
{
  register signed char *entry;
  int i;
  signed char *new_var;
  i = 0;
  entry = (signed char *)&g_GameState;
  while (i < 2)
  {
    if (entry[PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.keys)] == arg0)
    {
      new_var = entry;
      entry = &new_var[PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.keys) + 1];
      return *entry;
    }
    i++;
    entry += 2;
  }

  return -1;
}

int Overlay_FindAudioSlotIndex(int id) {
    int i = 0;
    char *ptr = (char *)&g_GameState;

    do {
        if (*(s8 *)(ptr + PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.keys)) == id) {
            return i;
        }
        i++;
        ptr += 2;
    } while (i < 2);

    return -1;
}
