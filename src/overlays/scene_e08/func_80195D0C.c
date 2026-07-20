/* BUG(original): the fade path reaches set_rot with `turn` holding the
 * 0x07FFFFFF comparison result, so the yaw is set to 1. */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
s32 FieldEng_TurnToward(s32 current, s32 target, s32 step);
s32 FieldEng_VecToAngle(s32 *from, s32 *to);
void func_800DFE94(void *pos, void *base, void *rot);
void func_80195D0C(char *ent, char *rec, s32 scratch)
{
  s32 new_var2;
  s32 turn;
  short new_var3;
  char *new_var;
  if ((*((s16 *) (rec + 0xA2))) >= (0 + 1))
  {
    s32 *dst = (s32 *) (rec + 0x60);
    char *v = *((char **) (rec + 0x84));
    new_var = rec - (-0xA9);
    if (0 != v)
    {
      dst[0] = *((s32 *) (v + 0x28));
      dst[2] = *((s32 *) (v + 0x30));
    }
    turn = FieldEng_TurnToward(*((s16 *) (ent + 0x3A)), (s16) FieldEng_VecToAngle(dst, (s32 *) (ent + 0x28)), *((s16 *) (rec + 0xA2)));
    new_var2 = turn & 0xFFF;
    new_var3 = new_var2;
    *((s16 *) (ent + 0x3A)) = new_var3;
    if ((*((u8 *) new_var)) != 0)
    {
      s32 lim = 0x07FFFFFF;
      turn = lim < (new_var2 = *((s32 *) (rec + 0x98)));
      if (turn)
      {
        *((s16 *) (rec - (-0xA2))) = 0;
        *((s16 *) (rec + 0xA4)) = 0;
        goto set_rot;
      }
    }
  }
  else
  {
    func_800DFE94(ent + 0x28, ent + 0x40, ent + 0x38);
    turn = FieldEng_TurnToward(*((s16 *) (ent + 0x3A)), *((s16 *) (ent + 0x3A)), *((s16 *) (rec + 0xA4)));
    set_rot:
    *((s16 *) (ent + 0x3A)) = turn & 0xFFF;

  }
}
