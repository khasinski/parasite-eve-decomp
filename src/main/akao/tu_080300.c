extern int g_AkaoInstrumentTable;

#include "pe1/akao.h"

void SeqOp_SetReturnPoint(void *ptr) {
    unsigned char **stream;
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    stream = (unsigned char **)ptr;
    cursor = *stream;
    value = *(*stream)++;
    next_cursor = cursor + 2;
    *stream = next_cursor;
    value |= cursor[1] << 8;

    *(short *)((char *)ptr + 0x5A) = 0xFF;
    *(short *)((char *)ptr + 0xE2) = 0;
    *(unsigned int *)((char *)ptr + 0x38) =
        *(unsigned int *)((char *)ptr + 0x38) | 0x1000;
    *(unsigned char **)((char *)ptr + 0x18) = next_cursor + (short)value;
}

void SeqOp_LoadInstrumentFromIndex(void *ptr)
{
  int value;
  register int base;
  base = *((unsigned short *) (((char *) ptr) + 0x5A));
  base = ((int) (&g_AkaoInstrumentTable)) + (base << 6);
  value = *((unsigned char *) (base + 0x8));
  *((short *) (((char *) ptr) + 0x10E)) = value;
  value = *((unsigned char *) (base + 0x9));
  *((short *) (((char *) ptr) + 0x110)) = value;
  value = *((unsigned char *) (base + 0xA));
  *((short *) (((char *) ptr) + 0x112)) = value;
  value = *((unsigned char *) (base + 0xB));
  *((short *) (((char *) ptr) + 0x114)) = value;
  value = *((unsigned char *) (base + 0xC));
  *((short *) (((char *) ptr) + 0x116)) = value;
  value = *((unsigned char *) (base + 0xD));
  *((int *) (((char *) ptr) + 0x100)) = value;
  value = *((unsigned char *) (base + 0xE));
  *((int *) (((char *) ptr) + 0x104)) = value;
  value = *((unsigned int *) (((char *) ptr) + 0xF4));
  base = *((unsigned char *) (base + 0xF));
  value |= 0xFF00;
  *((unsigned int *) (((char *) ptr) + 0xF4)) = value;
  *((int *) (((char *) ptr) + 0x108)) = base;
}

void sndTrackReadExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression = (signed char)*cursor;
}

void sndTrackAdjustExpression(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->expression += (signed char)*cursor;
}

void SeqOp_SetVibratoParams(void *ptr) {
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    cursor = *(unsigned char **)ptr;
    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    *(short *)((char *)ptr + 0x7E) = value;
    if (value == 0) {
        *(short *)((char *)ptr + 0x7E) = 0x100;
    }

    next_cursor = *(unsigned char **)ptr;
    *(unsigned char **)ptr = next_cursor + 1;
    *(short *)((char *)ptr + 0xE4) = (signed char)*next_cursor;
}

void SeqOp_SetTremoloParams(void *ptr) {
    unsigned char *cursor;
    int value;

    cursor = *(unsigned char **)ptr;
    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    *(short *)((char *)ptr + 0x82) = value;
    if (value == 0) {
        *(short *)((char *)ptr + 0x82) = 0x100;
    }

    *(short *)((char *)ptr + 0xE6) = 0;
    *(short *)((char *)ptr + 0x80) = 0;
    *(short *)((char *)ptr + 0x84) = 1;
}

void SeqOp_StopTremolo(void *ptr) {
    *(short *)((char *)ptr + 0x82) = 0;
}

void sndTrackReadDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune = (signed char)*cursor;
}

void sndTrackAdjustDetune(AkaoTrack *track) {
    unsigned char *cursor = track->pc;

    track->pc = cursor + 1;
    track->detune += (signed char)*cursor;
}
