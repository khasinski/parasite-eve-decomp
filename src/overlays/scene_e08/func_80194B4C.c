
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
void func_80194B4C(s32 arg0, s16 *rec, u16 *p)
{
  u8 *b = (u8 *) p;
  u16 *q;
  b[0x22] += 0x10;
  p[14] -= p[15];
  p[15] += 0xB;
  p[16] += 8;
  q = p;
  if (((s16) p[16]) >= 0x81)
  {
    p[16] = 0x80;
  }
  if (((s16) p[14]) < 0)
  {
    p[14] = 0;
  }
  if (b[0x22] >= 0xC9)
  {
    b[0x22] = 0xC8;
  }
  {
    s32 lvl = ((u8 *) q)[0x22] >> 4;
    if (lvl >= 4)
    {
      q[10] += 0xC8;
    }
  }
  if (rec[1] == 0x78)
  {
    ((s8 *) rec)[1] = 2;
  }
}
