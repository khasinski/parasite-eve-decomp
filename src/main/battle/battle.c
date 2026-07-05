typedef signed short s16;
int Gte_Atan2(int arg0, int arg1);

typedef unsigned short u16;

extern void *g_ActiveActor;

int rand(void);

int Battle_CalcRelativeAngle(void *arg0, void *arg1)
{
  register int angle;
  int wrapped;
  angle = 0x800 - Gte_Atan2((*((int *) (((char *) arg0) + 0x28))) - (*((int *) (((char *) arg1) + 0x28))), (*((int *) (((char *) arg0) + 0x30))) - (*((int *) (((char *) arg1) + 0x30))));
  angle = angle << 16;
  angle = angle >> 16;
  angle += *((s16 *) (((char *) arg1) + 0x3A));
  if (angle >= 0)
  {
    wrapped = angle;
  }
  else
  {
    wrapped = angle + 0xFFF;
  }
  wrapped >>= 12;
  wrapped <<= 12;
  return (s16) (angle - wrapped);
}

void Battle_CheckDropChance(void)
{
    void *ctx;
    int flags;
    u16 chance;

    ctx = g_ActiveActor;
    flags = *(int *)(*(char **)((char *)ctx + 0x68) + 0x10);
    if (flags & 0x10000) {
        chance = *(u16 *)((char *)ctx + 0x22);
        if ((rand() % 100) < chance) {
            *(short *)((char *)g_ActiveActor + 0x10) = 0x2328;
        }
    }
}
