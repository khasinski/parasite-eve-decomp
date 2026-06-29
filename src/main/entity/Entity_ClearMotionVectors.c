#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearMotionVectors(void) {
    g_CurrentEntity->motion_x = 0;
    g_CurrentEntity->motion_y = 0;
    g_CurrentEntity->motion_z = 0;
    g_CurrentEntity->accel_x = 0;
    g_CurrentEntity->accel_y = 0;
    g_CurrentEntity->accel_z = 0;
    return 1;
}
