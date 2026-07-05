/* MASPSX_FLAGS: --expand-div */

typedef unsigned char u8;
typedef unsigned int u32;

#include "pe1/akao.h"

extern int g_AkaoInstrumentTable;
void SeqOp_SetVoiceInstrument(void *track_arg, void *src_arg, int arg2);

extern int g_SpuActiveVoiceMask;
extern int g_AkaoVoiceMaskScratch;
extern int g_AkaoTrack34Mask;
extern int g_AkaoTrack38Mask;
extern int g_AkaoTrack3CMask;

extern char *g_AkaoCurTrack;

typedef unsigned char u8_1;
typedef unsigned short u16;
typedef unsigned int u32_1;

void SeqOp_SetVoiceInstrument(void *track_arg, void *src_arg, int arg2) {
    u8 *track = track_arg;
    u8 *src = src_arg;
    int flags;
    int value;
    u32 flags2;
    int tail_value;

    *(int *)(track + 0xF8) = arg2;
    *(int *)(track + 0xFC) = *(int *)(src + 4);
    *(short *)(track + 0x10E) = src[8];
    *(short *)(track + 0x110) = src[9];
    *(short *)(track + 0x112) = src[10];
    *(short *)(track + 0x114) = src[11];
    *(int *)(track + 0x100) = src[13];

    flags = *(int *)(track + 0x38);
    value = src[14];
    *(int *)(track + 0x104) = value;

    if ((flags & 0x200) != 0) {
        *(u32 *)(track + 0xF4) |= 0x1BB80;
    } else {
        unsigned int v = src[12];
        short *dst = (short *)(track + 0x116);

        *dst = v;
        flags2 = *(u32 *)(track + 0xF4);
        tail_value = src[15];
        *(u32 *)(track + 0xF4) = flags2 | 0x1FF80;
        *(int *)(track + 0x108) = tail_value;
    }
}

void Akao_SetNotePitch(AkaoTrack *track, int arg1) {
    int ptr;

    track->note_pitch = arg1;
    ptr = (int)&g_AkaoInstrumentTable + (arg1 << 6);
    SeqOp_SetVoiceInstrument(track, ptr, *(int *)ptr);
}

void SeqOp_DeactivateVoice(char *ptr, int mask) {
    int *first = &g_SpuActiveVoiceMask;

    mask = ~mask;
    *first &= mask;
    g_AkaoTrack34Mask &= mask;
    g_AkaoTrack38Mask &= mask;
    g_AkaoTrack3CMask &= mask;
    g_AkaoVoiceMaskScratch &= mask;
    *(int *)(ptr + 0x2C) = 0;
    *(int *)(ptr + 0x28) = 0;
}

void SeqOp_SetTempo(void *ptr)
{
  unsigned char *stream = *((unsigned char **) ptr);
  char *track;
  register unsigned int value;
  *((unsigned char **) ptr) = stream + 1;
  track = g_AkaoCurTrack;
  ;
  *((unsigned int *) (track + 0x20)) = (*stream) << 16;
  stream = *((unsigned char **) ptr);
  *((unsigned char **) ptr) = stream + 1;
  stream = (unsigned char *) ((unsigned int) (*stream));
  value = *((unsigned int *) (track + 0x20));
  *((short *) (track + 0x52)) = 0;
  value |= ((unsigned int) stream) << 24;
  *((unsigned int *) (track + 0x20)) = value;
}

void SeqOp_SetPitchSlideTarget(void *ptr) {
    register void *stream asm("$5");
    u8_1 *pc;
    register int value asm("$2");

    stream = ptr;

    {
        register u8_1 *pc0 asm("$2");
        register char *track asm("$3");

        pc0 = *(u8_1 **)stream;
        *(u8_1 **)stream = pc0 + 1;
        track = g_AkaoCurTrack;
        value = pc0[0];
        *(u16 *)(track + 0x52) = value;
        if (value == 0) {
            value = 0x100;
            *(u16 *)(track + 0x52) = value;
        }
    }

    {
        register int high asm("$3");
        char *track;
        int current;

        pc = *(u8_1 **)stream;
        *(u8_1 **)stream = pc + 1;
        value = pc[0];
        *(u8_1 **)stream = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = *(u32_1 *)(track + 0x20);
        current &= 0xFFFF0000;
        *(u32_1 *)(track + 0x20) = current;
        *(u32_1 *)(track + 0x24) = (value - current) / *(u16 *)(track + 0x52);
    }
}
