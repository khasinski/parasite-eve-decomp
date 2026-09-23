#include "pe1/battle.h"
#include "pe1/gte.h"

typedef struct FacingMotion {
    u8 state;
    u8 phase;
    u16 amplitude;
    s16 angle;
} FacingMotion;

typedef struct FacingCore {
    u32 flags;
    u8 pad04;
    s8 status05;
    u8 pad06[0x16];
    u8 actions[0x80];
    u8 pad9C[8];
    FacingMotion motion;
    u8 padAA[0x12];
    u8 mode;
    u8 previous_mode;
    u8 step_flag;
    u8 padBF;
    s32 previous_frame;
    s32 previous_step;
    s32 previous_delta;
} FacingCore;

extern Combatant *D_8009D278;
extern BattleEntity *D_8009D254;
int Battle_CalcRelativeAngle(BattleEntity *, BattleEntity *);
void Entity_SetActionMode(BattleEntity *, int);

void Battle_UpdateEntityFacing(BattleEntity *entity)
{
    FacingCore *core = (FacingCore *)entity->core;
    s16 direction;
    int mode;
    int phase;
    int value;
    BattleEntity *call_entity;
    FacingMotion *motion;

    if (core->status05 != 0 || (core->flags & 0xE)) return;
    if (D_8009D278->stateFlags & 0x80000) {
        if (core->actions[(core->flags >> 17) & 0x70]) goto update_angle;
    }
    if (core->mode == 1) {
        core->mode++;
        core->previous_mode = entity->actionMode;
        core->step_flag = (entity->entityFlags >> 9) & 1;
        core->previous_frame = entity->animFrame;
        core->previous_step = entity->animPrev.fixed;
        core->previous_delta = entity->animStep;
    }
update_angle:
    direction = (s16)Battle_CalcRelativeAngle(D_8009D254, entity);
    call_entity = entity;
    if (direction < 0x401) goto straight;
    if (direction < 0xC00) goto turn;
straight:
    mode = 0;
    goto set_mode;
turn:
    mode = 1;
set_mode:
    Entity_SetActionMode(call_entity, mode);
    entity->animStep = 0x10000;
    entity->entityFlags |= 0x1000;
    if (D_8009D278->stateFlags & 0x80000) return;
    if (!(core->flags & 0x100000)) return;
    if (core->motion.state == 0) return;
    motion = &core->motion;
    switch (core->motion.state) {
    case 2:
        core->motion.phase = 2;
        core->motion.amplitude = 0x46;
        break;
    case 3:
        core->motion.phase = 5;
        core->motion.amplitude = 0x14;
        break;
    case 1:
        core->motion.amplitude = 0x190;
        break;
    }
    phase = (u16)D_8009D254->facingAngle + 0x800;
    motion->angle = phase;
    value = rsin((s16)phase);
    entity->posX.fixed += ((u32)(motion->amplitude * value) << 4);
    value = rcos(motion->angle);
    entity->posZ.fixed += ((u32)(motion->amplitude * value) << 4);
}
