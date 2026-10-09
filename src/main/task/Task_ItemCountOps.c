#include "pe1/menu_inventory.h"
#include "pe1/field_actor.h"
extern FieldActor *g_CurrentEntity[];

int Inv_AddItem(int arg0);

int Inv_CountByValue(int arg0);

int Task_ScaleAnimValue(int **arg0) {
    int value = *arg0[0];
    int scale = (s16)g_CurrentEntity[0]->render_object.hit_cylinder.radius * 2;
    RenderObjectHeader *dst = g_CurrentEntity[0]->render_object.header;

    dst->shadow_radius = (scale * value) >> 16;
    return 1;
}

int Task_ClampMenuRange(int **arg0) {
    Menu_ClampRange(*arg0[0]);
    return 1;
}

int Task_GiveItem(int **arg0) {
    *arg0[1] = Inv_AddItem(*arg0[0]);
    return 1;
}

int Task_CountItemsByValue(int **arg0) {
    *arg0[1] = Inv_CountByValue(*arg0[0]);
    return 1;
}
