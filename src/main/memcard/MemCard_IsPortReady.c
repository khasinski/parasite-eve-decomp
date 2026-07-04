
extern int g_MemCardCommandByte;
extern void *g_MemCardObjArray;
int MemCard_IsPortReady(int arg0)
{
  register int index;
  int offset;
  int addr;
  if (g_MemCardCommandByte != 0)
  {
    index = arg0 >> 4;
    offset = ((index << 4) - index) << 4;
    addr = offset + ((int) g_MemCardObjArray);
    return (*((unsigned char *) (addr + 0xE8))) == 8;
  }
  return 0;
}
