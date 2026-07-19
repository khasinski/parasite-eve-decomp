
typedef unsigned char u8;
typedef unsigned int u32;
int func_800DFB78(u8 *arg0)
{
  register u8 *state;
  u8 *obj;
  u8 *new_var;
  register u32 flags;
  register u32 masked;
  register u8 *child;
  u32 word;
  u32 byte;
  int phase;
  byte = arg0[0x19];
  obj = (u8 *) (*((u32 *) (((u8 *) arg0) + 0x8)));
  state = arg0 + 0xC;
  word = 4;
  if ((byte & word) && ((*((u8 *) (((u8 *) obj) + 0xE))) < 2))
  {
    return 1;
  }
  if (state[0xD] & 2)
  {
    flags = *((u32 *) (((u8 *) obj) + 0x98));
    if (flags & 0x80000)
    {
      masked = flags & 0xFFF7FFFF;
      *((u32 *) (((u8 *) obj) + 0x98)) = masked;
      return 1;
    }
  }
  if (state[0xD] & 1)
  {
    flags = *((u32 *) (((u8 *) obj) + 0x98));
    if (flags & 0x40000)
    {
      masked = flags & 0xFFFBFFFF;
 do { *((u32 *) (((u8 *) obj) + 0x98)) = masked; return 1; } while (0);
    }
  }
  child = (u8 *) (*((u32 *) (((u8 *) obj) + 0x0)));
  if (child != 0)
  {
    new_var = ((u8 *) child) + 0x0;
    word = *((u32 *) new_var);
    if (word & 0x1800)
    {
      return 1;
    }
    phase = (word >> 1) & 7;
    if (phase > 0)
    {
      return 1;
    }
  }
  return 0;
}
