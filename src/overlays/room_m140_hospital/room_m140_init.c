#include "../room_lib/room_lib.h"

extern char RoomLib_TableA_8018F0D0[];
extern char RoomLib_TableB_8018F078[];
extern char g_RoomInitList[];
extern char D_80190E60[];
extern char g_RoomSpawnLayout[];

int RoomLib_CloseTarget_8018F1A4(RoomEnt *obj);

ROOMLIB_RETURN_ZERO(func_8018F070)
ROOMLIB_PLANT_TABLE(RoomLib_PlantTable_8018F078, RoomLib_TableB_8018F078)
ROOMLIB_SPAWN6(RoomLib_Spawn6_8018F0A4)
ROOMLIB_REGISTER_TABLE(RoomLib_RegisterTable_8018F0D0, RoomLib_TableA_8018F0D0)

int func_8018F118(RoomEnt *obj)
{
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(obj, D_80190E60);
        result |= func_800C2758(obj, g_RoomInitList, g_RoomSpawnLayout);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F1A4(obj);
    }

    return 0;
}

ROOMLIB_CLOSE_TARGET(RoomLib_CloseTarget_8018F1A4)

int func_8018F228(void)
{
    return 0;
}
