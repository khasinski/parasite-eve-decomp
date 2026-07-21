
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
s32 func_80020CE4(void);
s32 func_8001AA78(void);
extern char *D_8009D254;
extern u32 D_800BCF88;
s32 func_80192D0C(char *obj)
{
  s32 *out;
  char *sub = obj + 0xC;
  char *e;
  obj[0] = 4;
  if (obj[0x44] != 0)
  {
    char *node = *((char **) D_8009D254);
    if (node != 0)
    {
      if ((*((s16 *) (node + 0xC))) > 0)
      {
        func_80020CE4();
      }
    }
    ;
    *((s32 *) (D_8009D254 + 0x98)) &= 0xFFFEFFFF;
    *((u16 *) (D_8009D254 + 0x250)) &= 0xFBFF;
    *((s32 *) (D_8009D254 + 0x1D8)) = 0;
    *((s16 *) (D_8009D254 + 0x1DC)) = 0;
    func_8001AA78();
    e = D_8009D254;
    *((s32 *) (e + 0x68)) = 0;
    *((s32 *) (e + 0x6C)) = 0;
    *((s32 *) (e + 0x70)) = 0;
    *((s32 *) (e + 0x78)) = 0;
    *((s32 *) (e + 0x7C)) = 0;
    out = (s32 *) (e + 0x40);
    *((s32 *) (e + 0x80)) = 0;
    *out = *((s32 *) (e + 0x28));
    *((s32 *) (e + 0x44)) = *((s32 *) (e + 0x2C));
    *((s32 *) (e + 0x48)) = *((s32 *) (e + 0x30));
    D_800BCF88 |= 0x80;
    sub[0x38] = 0;
  }
  return 0;
}
