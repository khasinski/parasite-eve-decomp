/* Shared no-op continuation: this function returns through the D_80192344
 * stub that other handlers store as their resume target. */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
unsigned long FieldEng_VecToAngle(s32 *from, s32 *to);
extern char *D_8009D254;
extern s16 D_800966EC[][2];
extern char D_80192344[];
void func_80192238(char *obj)
{
  char *cur;
  s32 *notify;
  s16 *entry;
  if (1)
  {
    cur = D_8009D254;
    if ((*((u8 *) (cur + 0xE))) < 4)
    {
      *((s32 *) (cur + 0x98)) &= 0xFFF3FFFF;
      notify = *((s32 **) (obj + 0x10));
      *((char **) (obj + 0xC)) = D_80192344;
      if (notify != 0)
      {
        *notify = 2;
      }
      if ((*((s16 *) (obj + 0x44))) == (-1))
      {
        *((s16 *) (obj + 0x44)) = FieldEng_VecToAngle((s32 *) ((*((char **) (obj + 0x8))) + 0x28), (s32 *) (D_8009D254 + 0x28));
      }
      if ((*((s16 *) (obj + 0x48))) == (-1))
      {
        *((s16 *) (obj + 0x48)) = *((u16 *) (obj + 0x44));
        *((s16 *) (obj + 0x48)) = (*((s16 *) (obj + 0x48))) + 0x800;
      }
      {
        s32 sin_v;
        s32 cos_v;
        u16 turn_back;
        entry = D_800966EC[(*((u16 *) (obj + 0x44))) & 0xFFF];
        sin_v = entry[1];
        *((s16 *) (obj + 0x1E)) = 0;
        *((s16 *) (obj + 0x1C)) = sin_v;
        cos_v = *((s32 *) entry);
        ;
        *((s16 *) (obj + 0x22)) = 0;
        *((s16 *) (obj + 0x26)) = 0;
        *((s16 *) (obj + 0x2A)) = 0;
        *((s16 *) (obj + 0x20)) = cos_v;
        *((u16 *) (obj + 0x2C)) = *((u16 *) (obj + 0x1C));
        *((s16 *) (obj + 0x24)) = 0x1000;
        *((s16 *) (obj + 0x28)) = -(*((u16 *) (obj + 0x20)));
      }
    }
  }
}
