#include "common.h"
#include "pe1/game_state.h"

/* The raw views retain the retail addressing of the two audio-key records. */
int Overlay_RegisterAudioSlot(int arg0, int arg1, int arg2, int arg3) {
    char *base;
    char *ptr;

    base = (char *)&g_GameState;
    ptr = &base[arg0 * 2];
    ptr[0xDC] = (char) arg1;
    ptr[0xDD] = (char) arg2;
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
    if (entry[0xDC] == arg0)
    {
      new_var = entry;
      entry = &new_var[0xDD];
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
        if (*(s8 *)(ptr + 0xDC) == id) {
            return i;
        }
        i++;
        ptr += 2;
    } while (i < 2);

    return -1;
}
