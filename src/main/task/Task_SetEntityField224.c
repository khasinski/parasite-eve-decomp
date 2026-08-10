#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

int Task_SetEntityField224(short **arg0) {
    FieldActor *entity;
    int value;

    entity = g_CurrentEntity[0];
    value = (*arg0)[1];
    entity->render_object.table_value70 = value;
    return 1;
}
