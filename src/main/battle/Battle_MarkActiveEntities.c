/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle.h"

/* Unsized declarations preserve the original absolute accesses with -G8. */
extern s8 D_8009D2B0[];
extern Combatant *D_8009D278[];
extern BattleEntity *D_8009D20C[];
extern BattleEntity *D_8009D254[];
extern s8 D_8009CE6C;

inline static void Mark(BattleEntity *entity)
{
  int kind = (s8)((Combatant *)entity->core)->field04.bytes.field05;
  if (kind == 1)
  {
    entity = entity->parent;
  }
  else
    if (kind == 4)
  {
    BattleEntity *active;
    entity = D_8009D20C[0];
    if (!entity)
      return;
    active = D_8009D254[0];
    while (entity != 0)
    {
      if (((entity != active) && ((entity->core) != 0)) && (((s8)((Combatant *)entity->core)->field04.bytes.field05) == 4))
      {
        entity->renderObject.flags_9C |= 0x20;
      }
      entity = entity->next;
    }

    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

inline static void MarkCached(BattleEntity *entity, BattleEntity *active)
{
  int kind = (s8)((Combatant *)entity->core)->field04.bytes.field05;
  if (kind == 1)
  {
    entity = entity->parent;
  }
  else
    if (kind == 4)
  {
    entity = D_8009D20C[0];
    while (entity != 0)
    {
      if (((entity != active) && ((entity->core) != 0)) && (((s8)((Combatant *)entity->core)->field04.bytes.field05) == 4))
      {
        entity->renderObject.flags_9C |= 0x20;
      }
      entity = entity->next;
    }

    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

inline static void MarkKinds(BattleEntity *entity, int group_kind, int linked_kind, BattleEntity *active)
{
  int kind = (s8)((Combatant *)entity->core)->field04.bytes.field05;
  if (kind == linked_kind) entity = entity->parent;
  else if (kind == group_kind) {
    entity = D_8009D20C[0];
    while (entity != 0) {
      if (entity != active && entity->core != 0 && (s8)((Combatant *)entity->core)->field04.bytes.field05 == group_kind)
        entity->renderObject.flags_9C |= 0x20;
      entity = entity->next;
    }
    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

void Battle_MarkActiveEntities(BattleTarget *table, int selected_index)
{
  int mode;
  s16 center;
  s16 lower;
  s16 upper;
  int center_copy;
  int low_copy, high_copy;
  int inside;
  int linked_kind;
  int group_kind;
  BattleEntity *cached_active;
  unsigned char slot;
  BattleEntity *active;
  if (D_8009D2B0[0] == 0)
  {
    return;
  }
  mode = (D_8009D278[0]->action->turnWord >> 6) & 3;
  D_8009CE6C = 0;
  switch (mode)
  {
    case 0:
      Mark(table[(s8) selected_index].actor);
      break;

    case 2:
      center = table[(s8) selected_index].angle;

      if (center < (-0x600))
    {
      lower = center + 0x200;
      upper = center + 0xE00;
    }
    else
      if (center < 0x600)
    {
      lower = center - 0x200;
      upper = center + 0x200;
    }
    else
    {
      lower = center - 0xE00;
      upper = center - 0x200;
    }
      if (table[0].actor == 0)
    {
      return;
    }
      slot = 0;
      high_copy = upper;
      low_copy = lower;
      center_copy = center;
      inside = center < 0x600;
      {
        /* Match debt: one t3 pin and this empty scheduling barrier keep
         * the group-kind constant before the active-entity load. */
        register int range_group asm("$11") = 4;
        asm volatile("" : : "r"(range_group));
      }
      active = D_8009D254[0];
      cached_active = active;
      for (; table[slot].actor != 0; slot++)
    {
      s16 angle;
      if ((center_copy >= (-0x600)) && inside)
      {
        angle = table[slot].angle;
        if ((angle < low_copy) || (angle > high_copy))
        {
          continue;
        }
      }
      else
      {
        angle = table[slot].angle;
        if ((angle < high_copy) && (angle > low_copy))
        {
          continue;
        }
      }
      MarkCached(table[slot].actor, cached_active);
    }

      break;

    case 1:

    case 3:
      if (table[0].actor == 0)
    {
      return;
    }
      slot = 0;
      linked_kind = 1;
      group_kind = 4;
      /* Match debt: materialize both kinds before loading the active actor. */
      asm volatile("" : : "r"(linked_kind), "r"(group_kind));
      active = D_8009D254[0];
      for (; table[slot].actor != 0; slot++)
    {
      MarkKinds(table[slot].actor, group_kind, linked_kind, active);
    }

      break;

  }

}
