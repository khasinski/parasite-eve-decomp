
extern unsigned short D_800BCE9E;
extern unsigned short D_800BCE8A;
extern unsigned char D_800B0DB1;
int func_8006A2E8(int arg0, unsigned int value)
{
  register unsigned int v;
  if (value < 16)
  {
    v = value;
    D_800B0DB1 = (D_800BCE8A = (D_800BCE9E = v));
  }
  return 0;
}
