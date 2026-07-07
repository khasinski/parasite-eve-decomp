
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

int Entity_SetCurrentFlags(int **arg0) {
    g_CurrentEntity[0]->flags |= (*arg0)[0];
    return 1;
}
