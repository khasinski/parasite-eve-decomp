#include "common.h"
#include "pe1/battle.h"
int Gte_Atan2(int arg0, int arg1);

extern void *g_ActiveActor;

int rand(void);

#define COMBATANT_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))
#define ENTITY_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(BattleEntity, member)))

int Battle_CalcRelativeAngle(void *arg0, void *arg1)
{
  register int angle;
  int wrapped;
  angle = 0x800 - Gte_Atan2(
      ENTITY_FIELD(arg0, int, posX) - ENTITY_FIELD(arg1, int, posX),
      ENTITY_FIELD(arg0, int, posZ) - ENTITY_FIELD(arg1, int, posZ));
  angle = angle << 16;
  angle = angle >> 16;
  angle += ENTITY_FIELD(arg1, s16, facingAngle);
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
    flags = ACTION_FIELD(COMBATANT_FIELD(ctx, char *, action), int, turnWord);
    if (flags & 0x10000) {
        chance = COMBATANT_FIELD(ctx, u16, stat22);
        if ((rand() % 100) < chance) {
            COMBATANT_FIELD(g_ActiveActor, short, hpAlive) = 0x2328;
        }
    }
}

#undef ENTITY_FIELD
#undef ACTION_FIELD
#undef COMBATANT_FIELD
