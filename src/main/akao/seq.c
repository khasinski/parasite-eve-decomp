#include "include_asm.h"

typedef unsigned int u32;

extern char *g_AkaoCurTrack;

typedef unsigned short u16;
typedef unsigned int u32_2;
extern u16 *D_8009D240;
extern char *D_8009D260;

extern u32 g_SpuActiveVoiceMask;
extern char g_AkaoVoiceChannelTable[];

void Seq_ClearTrackVoiceId(char *arg0, unsigned int arg1) {
    unsigned int i;
    int bit;
    int reset;
    char *ptr;
    char *state;

    if (arg1 < 24) {
        i = 0;
        reset = 24;
        state = g_AkaoCurTrack;
        bit = 1;
        ptr = arg0 + 0xF0;
        do {
            if (*(int *)ptr == arg1) {
                *(int *)ptr = reset;
                *(u32 *)(state + 0x14) &= ~(bit << i);
            }
            i++;
            ptr += 0x11C;
        } while (i < 24);
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/seq", Seq_StartNestedStreams);

void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2)
{
  u16 *new_var2;
  u32_2 index;
  u16 *table;
  register char *addr;
  int new_var;
  u16 value;
  int result;
  index = arg2;
  index &= 0x3FF;
  index <<= 1;
  table = D_8009D240;
  result = (int) table;
  addr = (char *) (index << (1 & 0xFF));
  addr += result;
  new_var = 0;
  value = *((u16 *) addr);
  if (value != 0xFFFF)
  {
    result = (int) (D_8009D260 + value);
  }
  else
  {
    result = new_var;
  }
  *arg0 = result;
  index++;
  table = D_8009D240;
  addr = (char *) (index << 1);
  addr += (int) table;
  new_var2 = (u16 *) addr;
  value = *new_var2;
  if (value != 0xFFFF)
  {
    result = (int) (D_8009D260 + value);
  }
  else
  {
    result = new_var;
  }
  *arg1 = result;
}

void Seq_MarkDirtyTracks(char *arg0) {
    u32 mask;
    u32 bit;

    mask = *(u32 *)(g_AkaoCurTrack + 4);
    if (mask != 0) {
        bit = 1;
        arg0 += 0xF4;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u32 *)arg0 |= 3;
            }
            arg0 += 0x11C;
            bit <<= 1;
        } while (mask != 0);
    }
}

void Spu_MarkActiveVoicesDirty(void) {
    u32 mask;
    u32 bit;
    char *base;
    char *ptr;

    mask = g_SpuActiveVoiceMask;
    base = g_AkaoVoiceChannelTable;
    if (mask != 0) {
        bit = 0x1000;
        ptr = base + 0xF4;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u32 *)ptr |= 3;
            }
            ptr += 0x11C;
            bit <<= 1;
        } while (mask != 0);
    }
}
