
typedef signed short s16;
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
