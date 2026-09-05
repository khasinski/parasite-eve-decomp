/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

extern int func_80191500(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191160
#define ROOMLIB_ADVANCE_ARC_RESET func_80191500
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
