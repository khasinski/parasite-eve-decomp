#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/random.h"

/* Direction helpers for field effects: shortest turn between two angles,
 * eased turning, point distance and the alternating scatter offsets. */

extern u16 D_800E21C8;

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

int func_800CFE94(s16 *from, s16 *to)
{
    int dx = to[0] - from[0];
    int dy = to[1] - from[1];
    int dz = to[2] - from[2];
    int length = SquareRoot0((dx * dx) + (dy * dy) + (dz * dz));

    if (length == 0) {
        length = 1;
    }

    return length;
}

void func_800CFF0C(s16 *out)
{
    s16 *out_reg;
    int y;
    int phase;
    int temp;
    int z;

    out_reg = out;
    D_800E21C8 = (D_800E21C8 + 1) & 7;
    temp = rand() & 0x1FF;
    phase = D_800E21C8;
    temp += (phase << 9) & 0xC00;
    phase &= 1;
    y = temp + 0x100;
    if ((phase & 1) != 0) {
        z = (rand() & 0x1FF) + 0x100;
    } else {
        z = -(rand() & 0x1FF) - 0x100;
    }

    out_reg[0] = 0;
    out_reg[1] = y;
    out_reg[2] = z;
}

void func_800CFFAC(s16 *out)
{
    s16 *out_reg;
    int x;
    int phase;
    int temp;
    int y;

    out_reg = out;
    D_800E21C8 = (D_800E21C8 + 1) & 7;
    temp = rand() & 0x1FF;
    phase = D_800E21C8;
    temp += (phase << 9) & 0xC00;
    phase &= 1;
    x = temp + 0x100;
    if ((phase & 1) != 0) {
        y = (rand() & 0x1FF) + 0x100;
    } else {
        y = -(rand() & 0x1FF) - 0x100;
    }

    out_reg[0] = x;
    out_reg[1] = y;
    out_reg[2] = 0;
}
