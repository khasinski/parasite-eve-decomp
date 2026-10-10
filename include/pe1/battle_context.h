#ifndef PE1_BATTLE_CONTEXT_H
#define PE1_BATTLE_CONTEXT_H

struct FieldActor;
int Battle_GetContextField(int field);
int Battle_GetEnemyContextField(struct FieldActor *actor, int field);
void Battle_SetContextField(int field, int value);

#endif
