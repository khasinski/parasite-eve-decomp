/* Script opcodes from 0x80019C04 to 0x8001A214: roam flag, angle to a point,
 * spawn snap, scene exit and save overlay, game-time conversion, actor parent
 * links and the trigonometry helpers. All are default-profile handlers that
 * read the current actor through absolute addresses. */
#include "common.h"
#include "pe1/field_actor.h"

#define NULL ((void *)0)

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_FieldActorListHead;
extern FieldActor *g_PlayerEntity;
extern int g_GameTimeTable[];

int rsin(int arg0);
int rcos(int arg0);
int ratan2(int arg0, int arg1);
int Math_IntSqrt(int value);
void Task_SignalExit(void);
void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3);
void Render_SetupColorTable(int arg0, int arg1, short *arg2);

int Task_ClearEntityRoamFlag(void) {
    g_CurrentEntity->flags &= 0xFFFDFFFF;
    return 1;
}

int Task_SetEntityRoamFlag(void) {
    g_CurrentEntity->flags |= 0x20000;
    return 1;
}

int Task_GetAngleToPoint(int **arg0) {
    int angle;
    int dx;
    int dy;

    dx = (g_CurrentEntity->pos_x - *arg0[0]) >> 16;
    dy = (g_CurrentEntity->pos_z - *arg0[1]) >> 16;
    angle = 0x1400 - ratan2(dy, dx);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= g_CurrentEntity->rot_y;
    if (angle < 0) {
        angle += 0x1000;
    }
    *arg0[2] = angle;
    return 1;
}

int Task_SnapPosToSpawnPos(void) {
    FieldActor *actor;
    register int x asm("$2");
    int y;

    actor = g_CurrentEntity;
    x = actor->render_object.hit_cylinder.value0;
    y = actor->render_object.hit_cylinder.value2;
    x <<= 16;
    actor->pos_x = x;
    x = actor->render_object.hit_cylinder.value1;
    y <<= 16;
    actor->pos_z = y;
    x <<= 16;
    actor->pos_y = x;
    return 1;
}

int Task_SignalSceneExit(void) {
    Task_SignalExit();
    return 1;
}

int Task_DrawSaveOverlay(unsigned short **arg0) {
    Menu_SetTextCursorRect(*arg0[0], *arg0[1], *arg0[2], *arg0[3]);
    return 1;
}

int Task_ResetEntityAnimSlot(short **arg0) {
    short local[8];

    local[0] = -1;
    Render_SetupColorTable(*arg0[0], 1, local);
    return 1;
}

int Task_GetFloorTableValue(int **arg0) {
    *arg0[1] = g_GameTimeTable[*arg0[0] * 3];
    return 1;
}

typedef struct GameTimeArgs {
    s32 *seconds;
    s32 *hours;
    s32 *minutes;
    s32 *rest;
} GameTimeArgs;

s32 Task_ConvertSecondsToHMS(GameTimeArgs *arg0) {
    *arg0->hours = *arg0->seconds / 216000;
    *arg0->minutes = (*arg0->seconds % 216000) / 3600;
    *arg0->rest = (*arg0->seconds % 3600) / 60;
    return 1;
}

/* Script op: attach the current entity to the actor named by
 * (args[0], args[1]) as its parent (pos/rot follow when flags & 0x400000). */
s32 Task_SetEntityParentLink(s32 *args[]) {
    s32 key;
    FieldActor *node;

    key = *args[0];
    if (key == 0) {
        FieldActor *tmp;

        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto done;
        }
        node = tmp;
        goto link;
    }
    {
        u32 k = key;
        node = g_FieldActorListHead;
        if (node != NULL) {
loop:
        if ((node->type_id != k) || (node->sub_id != *args[1]) || (node->flags & 0x10)) {
            node = node->next;
            if (node != NULL) {
                goto loop;
            }
        }
            if (node != NULL) {
                goto link;
            }
        }
    }
done:
    return 1;
link:
    {
        FieldActor *cur = g_CurrentEntity;
        cur->parent = node;
        node->flags |= 0x100000;
        cur->flags |= 0x600000;
    }
    return 1;
}

int Task_ClearEntityParentLink(void) {
    FieldActor *current;
    FieldActor *it;

    current = g_CurrentEntity;
    it = g_FieldActorListHead;
    current->parent = 0;
    current->flags &= 0xFF9FFFFF;

    if (it != 0) {
        do {
            if (it != current) {
                if (it->parent == current->parent) {
                    return 1;
                }
            }
            it = it->next;
        } while (it != 0);
    }

    {
        FieldActor *tail_current;
        int clear_mask;
        tail_current = g_CurrentEntity;
        clear_mask = 0xFFEFFFFF;
        tail_current->parent->flags &= clear_mask;
    }
    return 1;
}

int Entity_ClearParentLink(void) {
    int clear_current = 0xFFEFFFFF;
    FieldActor *current = g_CurrentEntity;
    FieldActor *it = g_FieldActorListHead;

    current->flags &= clear_current;

    if (it != 0) {
        int clear_child = 0xFF9FFFFF;

        do {
        if (it->parent == current) {
            it->parent = 0;
                it->flags &= clear_child;
        }
            it = it->next;
        } while (it != 0);
    }

    return 1;
}

int Task_GetSin(int **arg0) {
    *arg0[1] = rsin(*arg0[0]) << 4;
    return 1;
}

int Task_GetCos(int **arg0) {
    *arg0[1] = rcos(*arg0[0]) << 4;
    return 1;
}

int Task_GetAtan2(int **arg0) {
    *arg0[2] = ratan2(*arg0[0], *arg0[1]);
    return 1;
}

int Task_GetSqrt(int **arg0) {
    *arg0[1] = Math_IntSqrt(*arg0[0]);
    return 1;
}

int Task_SetEntityFlag1000000(void) {
    g_CurrentEntity->flags |= 0x1000000;
    return 1;
}
