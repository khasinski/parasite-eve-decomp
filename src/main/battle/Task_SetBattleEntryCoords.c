#include "common.h"
#include "pe1/battle.h"

extern char *g_CurrentEntity;

#define CURRENT_ENTITY ((BattleEntity *)g_CurrentEntity)

void Battle_SetEntryCoords(char *arg0, int arg1, int arg2, int arg3);
void Render_SetEntryScrolled(int arg0, int arg1, int arg2, int arg3);
void Render_SetEntryMirrored(int arg0, int arg1, int arg2, int arg3);

int Task_SetBattleEntryCoords(int **arg0) {
    Battle_SetEntryCoords(g_CurrentEntity, *(unsigned char *)arg0[0], *arg0[1], *arg0[2]);
    return 1;
}

int func_80019904(void) {
    CURRENT_ENTITY->entityFlags &= -2;
    return 1;
}

int func_80019928(void) {
    CURRENT_ENTITY->entityFlags |= 1;
    return 1;
}

int func_8001994C(int **arg0) {
    Render_SetEntryScrolled(*arg0[0], *arg0[1], *arg0[2], *arg0[3]);
    return 1;
}

int func_8001998C(int **arg0) {
    Render_SetEntryMirrored(*arg0[0], *arg0[1], *arg0[2], *arg0[3]);
    return 1;
}

int func_800199CC(int **arg0) {
    CURRENT_ENTITY->renderObject.script_value9a = *arg0[0];
    CURRENT_ENTITY->renderObject.flags_9C |= 0x10;
    return 1;
}

int func_800199F8(void) {
    CURRENT_ENTITY->renderObject.flags_9C &= 0xFFEF;
    return 1;
}

int func_80019A1C(int **arg0) {
    CURRENT_ENTITY->renderObject.script_value9a = *arg0[0];
    CURRENT_ENTITY->renderObject.script_param97 = *arg0[1];
    CURRENT_ENTITY->renderObject.script_param98 = *arg0[2];
    CURRENT_ENTITY->renderObject.script_param99 = *arg0[3];
    CURRENT_ENTITY->renderObject.flags_9C |= 8;
    return 1;
}

int func_80019A9C(void) {
    CURRENT_ENTITY->renderObject.flags_9C &= 0xFFF7;
    return 1;
}

int func_80019AC0(void) {
    CURRENT_ENTITY->entityFlags |= 0x400;
    return 1;
}

int func_80019AE4(void) {
    CURRENT_ENTITY->entityFlags &= -0x401;
    return 1;
}
