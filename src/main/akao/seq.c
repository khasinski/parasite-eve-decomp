#include "include_asm.h"

#include "pe1/akao.h"

typedef unsigned int u32;

extern char *g_AkaoCurTrack;

typedef unsigned short u16;
typedef unsigned int u32_2;
extern u16 *D_8009D240;
extern char *D_8009D260;

extern u32 g_SpuActiveVoiceMask;
extern char g_AkaoVoiceChannelTable[];
extern u32 D_800BCD50;
extern u32 g_AkaoVoiceUpdateFlags;
extern AkaoTrack D_800BCC34[];
extern AkaoTrack D_800B8AC0[];

void Spu_ManageVoices(int arg0, u32 arg1);
void Seq_StartNestedTrack(AkaoTrack *track, void *source, u32 voice_mask, void *script);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Seq_ClearTrackVoiceId(AkaoTrack *track, unsigned int arg1) {
    unsigned int i;
    int bit;
    int reset;
    char *state;

    if (arg1 < 24) {
        i = 0;
        reset = 24;
        state = g_AkaoCurTrack;
        bit = 1;
        do {
            if (track->assigned_voice_index == arg1) {
                track->assigned_voice_index = reset;
                *(u32 *)(state + 0x14) &= ~(bit << i);
            }
            i++;
            track++;
        } while (i < 24);
    }
}

void Seq_StartNestedStreams(void *source, void *script_a, void *script_b) {
    register void *source_reg asm("$22");
    register void *script_a_reg asm("$20");
    register void *script_b_reg asm("$21");
    register AkaoTrack *track asm("$18");
    register u32 mask asm("$17");
    register int count asm("$16");
    register int active asm("$19");
    register int active_scan asm("$3");
    u32 check_mask;

    source_reg = source;
    script_a_reg = script_a;
    script_b_reg = script_b;

    if (script_a_reg == 0 && script_b_reg == 0) {
        return;
    }

retry:
    if (((u32 *)source_reg)[2] != 0) {
        Spu_ManageVoices(0, ((u32 *)source_reg)[2]);
    }

    track = D_800BCC34;
    active = D_800BCD50;
    mask = 0x800000;
    active_scan = active;

    if (script_a_reg != 0 && script_b_reg != 0) {
        count = 0xB;
        track--;
        mask = 0x400000;
        do {
            check_mask = mask << 1;
            check_mask |= mask;
            if ((active_scan & check_mask) == 0) {
                break;
            }
            count--;
            track--;
            if (count == 0) {
                goto no_slot;
            }
            mask >>= 1;
        } while (1);
    } else {
        count = 0xC;
        do {
            if ((active_scan & mask) == 0) {
                break;
            }
            count--;
            track--;
            if (count == 0) {
                goto no_slot;
            }
            mask >>= 1;
        } while (1);
    }

    goto have_slot;

no_slot:
    Spu_ManageVoices(0, 0x40000000);
    if (active == D_800BCD50) {
        active = 0x80000000;
    }
    if (count == 0 && active >= 0) {
        goto retry;
    }

have_slot:
    if (active < 0) {
        return;
    }

    if (script_a_reg != 0) {
        Seq_StartNestedTrack(track, source_reg, mask, script_a_reg);
        Seq_ClearTrackVoiceId(D_800B8AC0, track->assigned_voice_index);
    }

    if (script_b_reg != 0) {
        if (script_a_reg != 0) {
            track++;
            mask <<= 1;
        }
        Seq_StartNestedTrack(track, source_reg, mask, script_b_reg);
        Seq_ClearTrackVoiceId(D_800B8AC0, track->assigned_voice_index);
        if (script_a_reg != 0) {
            track->flags |= 0x10000;
        }
    }

    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}

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

void Seq_MarkDirtyTracks(AkaoTrack *track) {
    u32 mask;
    u32 bit;

    mask = *(u32 *)(g_AkaoCurTrack + 4);
    if (mask != 0) {
        bit = 1;
        do {
            if (mask & bit) {
                mask ^= bit;
                track->update_flags |= AKAO_VOICE_PARAM_VOLUME;
            }
            track++;
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
        bit = AKAO_SPU_VOICE_SFX_START_MASK;
        ptr = base + 0xF4;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u32 *)ptr |= AKAO_VOICE_PARAM_VOLUME;
            }
            ptr += sizeof(AkaoTrack);
            bit <<= 1;
        } while (mask != 0);
    }
}
