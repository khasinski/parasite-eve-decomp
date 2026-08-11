#include "pe1/battle_runtime.h"

void Battle_CopyPadStateToRecord(void) {
    BattleAction *action;
    unsigned int value;

    action = D_8009D278->action;
    action->field00 = D_800A76D8.field00;
    action->range = D_800A76D8.range;
    action->field04 = D_800A76D8.field04;
    action->actionId = D_800A76D8.actionId;
    action->field08 = D_800A76D8.field08;

    value = action->attackWord;
    value = (value & ~0x3ff) | (D_800A76D8.attackWord & 0x3ff);
    action->attackWord = value;
    value = (value & 0xfff003ff) | (D_800A76D8.attackWord & 0xffc00);
    action->attackWord = value;
    value = (value & 0xffcfffff) | (D_800A76D8.attackWord & 0x300000);
    action->attackWord = value;

    value = action->turnWord;
    value = (value & ~0xf) | (D_800A76D8.turnWord & 0xf);
    action->turnWord = value;
    value = (value & ~0x30) | (D_800A76D8.turnWord & 0x30);
    action->turnWord = value;
    value = (value & ~0xc0) | (D_800A76D8.turnWord & 0xc0);
    action->turnWord = value;
    value = (value & ~0x100) | (D_800A76D8.turnWord & 0x100);
    action->turnWord = value;
    value = (value & ~0x200) | (D_800A76D8.turnWord & 0x200);
    action->turnWord = value;
    value = (value & ~0x400) | (D_800A76D8.turnWord & 0x400);
    action->turnWord = value;
    value = (value & ~0x800) | (D_800A76D8.turnWord & 0x800);
    action->turnWord = value;
    value = (value & ~0x1000) | (D_800A76D8.turnWord & 0x1000);
    action->turnWord = value;
    value = (value & ~0x6000) | (D_800A76D8.turnWord & 0x6000);
    action->turnWord = value;
    value = (value & ~0x8000) | (D_800A76D8.turnWord & 0x8000);
    action->turnWord = value;
    value = (value & ~0x10000) | (D_800A76D8.turnWord & 0x10000);
    action->turnWord = value;
    value = (value & ~0x20000) | (D_800A76D8.turnWord & 0x20000);
    action->turnWord = value;
}
