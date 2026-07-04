
typedef unsigned short u16;
typedef unsigned int u32;
extern u16 *D_8009D240;
extern char *D_8009D260;
void Akao_LoadSamplePairFromIndex(int *arg0, int *arg1, int arg2)
{
  u16 *new_var2;
  u32 index;
  u16 *table;
  register char *addr;
  int new_var;
  u16 value;
  int result;
  index = arg2;
  index &= 0x3FF;
  index <<= 1;
  table = D_8009D240;
  result = (int) table;
  addr = (char *) (index << (1 & 0xFF));
  addr += result;
  new_var = 0;
  value = *((u16 *) addr);
  if (value != 0xFFFF)
  {
    result = (int) (D_8009D260 + value);
  }
  else
  {
    result = new_var;
  }
  *arg0 = result;
  index++;
  table = D_8009D240;
  addr = (char *) (index << 1);
  addr += (int) table;
  new_var2 = (u16 *) addr;
  value = *new_var2;
  if (value != 0xFFFF)
  {
    result = (int) (D_8009D260 + value);
  }
  else
  {
    result = new_var;
  }
  *arg1 = result;
}
