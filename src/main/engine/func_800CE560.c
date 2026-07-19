
typedef unsigned short u16;
int func_800CE560(char *out, int stride, int count, int value)
{
  int i;
  register char *entry;
  stride += 4;
  entry = out;
  entry = entry + 0xC;
  *((int *) (out + 8)) = value;
  *((int *) (out + 0)) = stride;
  *((int *) (out + 4)) = count;
  for (i = 0; i < count; i++)
  {
    *((u16 *) entry) = 0;
    entry += stride;
  }

  return (stride * count) + 0xC;
}
