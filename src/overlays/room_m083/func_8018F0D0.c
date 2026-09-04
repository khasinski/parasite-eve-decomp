#include "../room_lib/room_lib.h"

extern void RoomLib_ArmWindowVia_8018F2BC(void);

#define ROOMLIB_CONFIGURE_WINDOW_NAME func_8018F0D0
#define ROOMLIB_CONFIGURE_WINDOW_CALLBACK RoomLib_ArmWindowVia_8018F2BC
#include "../room_lib/RoomLib_ConfigureWindowHandler.inc"
