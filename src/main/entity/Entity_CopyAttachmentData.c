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
