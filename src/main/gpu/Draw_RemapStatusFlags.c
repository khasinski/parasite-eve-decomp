
extern int g_FieldPadBits;
int Draw_RemapStatusFlags(void)
{
  int value;
  register int later_value;
  register int bit2;
  int mask;
  value = g_FieldPadBits;
  mask = (value << 9) & 0x1000;
  if (value & 0x20)
  {
    mask |= 0x4000;
  }
  if (value & 0x40)
  {
    mask |= 0x8000;
  }
  if (value & 0x10)
  {
    mask |= 0x2000;
  }
  if (value & 0x20000000)
  {
    mask |= 0x20;
  }
  if (value & 0x40000000)
  {
    mask |= 0x40;
  }
  if (value & 0x10000000)
  {
    mask |= 0x10;
  }
  if (value < 0)
  {
    mask |= 0x80;
  }
  if (value & 0x04000000)
  {
    mask |= 0x4;
  }
  if (value & 0x08000000)
  {
    mask |= 0x8;
  }
  later_value = value;
  if (later_value & 0x01000000)
  {
    mask |= 0x1;
  }
  if (later_value & 0x02000000)
  {
    mask |= 0x2;
  }
  bit2 = later_value & 0x2;
  if (bit2)
  {
    mask |= 0x100;
  }
  if (value & 0x4)
  {
    later_value = 0x800;
    mask |= later_value;
  }
  return mask;
}
