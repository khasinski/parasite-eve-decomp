#include "common.h"

int func_800CFCF4(int from, int to, s16 *distance)
{
  unsigned char new_var;
  register int direction;
  register int delta;
  register int direction16;
  int diff;
  from &= 0xFFF;
  to &= 0xFFF;
  direction = -1;
  if (((unsigned int) to) < ((unsigned int) from))
  {
    direction = 1;
  }
  diff = from - to;
  new_var = diff < 0;
  delta = diff;
  if (new_var)
  {
    delta = -delta;
  }
  if (delta >= 0x801)
  {
    direction16 = (s16) direction;
    direction = -direction16;
    delta = 0x1000 - delta;
  }
  if (distance != 0)
  {
    *distance = delta;
  }
  return (s16) direction;
}

/* Return the shortest turn direction and write its unsigned angle distance. */
static inline int angle_direction(u16 *fromPtr, u16 *toPtr, s16 *distance) {
    int from = *fromPtr & 0xFFF;
    int to = *toPtr & 0xFFF;
    u8 negative;
    int direction;
    int delta;
    int direction16;
    int diff;

    direction = -1;
    if ((unsigned int)to < (unsigned int)from) direction = 1;
    diff = from - to;
    negative = diff < 0;
    delta = diff;
    if (negative) delta = -delta;
    if (delta >= 0x801) {
        direction16 = (s16)direction;
        direction = -direction16;
        delta = 0x1000 - delta;
    }
    *distance = delta;
    return (s16)direction;
}

void func_800CFD50(u16 *from, u16 *to, u16 speed) {
    s16 distance;
    int direction;
    int scaled;
    u16 old0, old1;
    s16 change;

    direction = angle_direction(&from[0], &to[0], &distance);
    scaled = distance * speed;
    if (scaled < 0) scaled += 0xFFF;
    old0 = to[0];
    distance = scaled >> 12;
    change = distance * direction;
    to[0] = old0 + change;

    direction = angle_direction(&from[1], &to[1], &distance);
    scaled = distance * speed;
    if (scaled < 0) scaled += 0xFFF;
    old1 = to[1];
    distance = scaled >> 12;
    change = distance * direction;
    to[1] = old1 + change;
}
