#include "pe1/battle.h"

#define ENTITY_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(BattleEntity, member)))
#define RENDER_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(RenderObjectEntity, member)))

int Math_IntSqrt(int value);

int Gte_Atan2(int arg0, int arg1);

int Battle_CalcDistToPlayer(void *arg0, void *arg1)
{
    int x;
    int y;

    x = ENTITY_FIELD(arg0, s16, renderObject.target_x) -
        ENTITY_FIELD(arg1, s16, posX.parts.integer);
    y = ENTITY_FIELD(arg0, s16, renderObject.target_z) -
        ENTITY_FIELD(arg1, s16, posZ.parts.integer);
    return Math_IntSqrt((x * x) + (y * y));
}

int Battle_CalcAngleToTarget(void *arg0, void *arg1)
{
    int x;
    int y;
    int angle;

    x = RENDER_FIELD(arg0, s16, target_x) << 16;
    y = RENDER_FIELD(arg0, s16, target_z) << 16;
    angle = Gte_Atan2(x - ((int *)arg1)[0], y - ((int *)arg1)[2]);
    return (s16)(angle + 0x800);
}

#undef RENDER_FIELD
#undef ENTITY_FIELD
