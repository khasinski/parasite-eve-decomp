#define READ_S16(addr) ((short)(*(addr)++ | (*(addr)++ << 8)))

typedef struct TrackLike
{
  char pad[0x56];
  unsigned short limit;
} TrackLike;
extern TrackLike *g_AkaoCurTrack;

void sndTrackJumpRelative(unsigned char **stream) {
    *stream += READ_S16(*stream);
}

void SeqOp_BranchIfLoopLimitReached(unsigned char **stream)
{
  register unsigned char **cursor;
  unsigned char *ptr;
  unsigned char *target;
  int value;
  int lo;
  unsigned char **new_var;
  int offset;
  cursor = stream;
  ptr = *cursor;
  *cursor = ptr + 1;
  value = ptr[0];
  new_var = cursor;
  if (((int) g_AkaoCurTrack->limit) >= value)
  {
    *cursor = ptr + 2;
    lo = ptr[1];
    target = ptr + 3;
    *cursor = target;
    offset = lo | (ptr[2] << 8);
    target += (short) offset;
    *cursor = target;
  }
  else
  {
    *new_var = ptr + 3;
  }
}
