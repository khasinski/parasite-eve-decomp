#include "../room_lib/room_lib.h"

extern void RoomLib_WindowHandler(void);

ROOMLIB_ARM_IF_WINDOW_VIA(RoomLib_ArmWindowVia_8018F2BC, RoomLib_WindowHandler)
