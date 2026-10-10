#include "pe1/player_entity.h"

extern FieldActor *g_FieldActorListHead;
extern FieldActor *g_CurrentEntity;

int Entity_CopyAttachmentData(int **arg0)
{
    int **args = arg0;
    FieldActor *selected;
    RenderObjectEntity *src;
    if (*args[0] == 0)
    {
        FieldActor *current = g_PlayerEntity;
        if (current == 0)
        {
            return 1;
        }
        selected = current;
    }
    else
    {
        int id = *args[0];
        selected = g_FieldActorListHead;
        if (selected == 0)
        {
            return 1;
        }
        while (selected != 0)
        {
            if (((selected->type_id == id) && (selected->sub_id == (*args[1]))) && ((selected->flags & 0x10) == 0))
            {
                break;
            }
            selected = selected->next;
        }

        if (selected == 0)
        {
            return 1;
        }
    }
    src = &selected->render_object;
    Render_SetObjectAnim(&g_CurrentEntity->render_object, src, *(short *)args[2]);
    g_CurrentEntity->parent = selected;
    return 1;
}

/* Adjacent script handlers share actor selection and render-object state. */
#include "pe1/pm.h"


extern int g_BattleModeState;

int Task_ResetEntityRenderObj(void) {
    Render_ClearObjectAnim(&g_CurrentEntity->render_object);
    g_CurrentEntity->parent = 0;
    return 1;
}

int Task_SetEntityField1E6(int **arg0) {
    g_CurrentEntity->render_object.table_index = *arg0[0];
    return 1;
}

int Menu_ClearModeState(void) {
    g_BattleModeState = 0;
    return 1;
}

int Menu_SetModeState8(void) {
    g_BattleModeState = 8;
    return 1;
}


void Pm_StopAllBoth(void);

int Menu_SetSlotEntry(int index, unsigned int limit, unsigned int slot);

int Pm_ScriptStopCurrentEntity(int **arg0) {
    Pm_Stop(*arg0[0], g_CurrentEntity, 0);
    return 1;
}

int Task_SetEntityTarget(int **arg0) {
    int **args = arg0;
    FieldActor *selected;


    if (*args[0] == 0) {
        selected = g_PlayerEntity;
    } else {
        int id = *args[0];

        selected = g_FieldActorListHead;
        while (selected != 0) {
            if (selected->type_id == id &&
                    selected->sub_id == *args[1] &&
                    ((selected->flags & 0x10) == 0)) {
                break;
            }
            selected = selected->next;
        }
    }

    Scene_FreeEntityTable(selected);
    return 1;
}

int Task_ResetRenderState(void) {
    Pm_StopAllBoth();
    return 1;
}

int Task_SetMenuSlotEntry(int **arg0) {
    Menu_SetSlotEntry(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}
