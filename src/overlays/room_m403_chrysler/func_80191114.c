/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_801914B4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191114
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914B4
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
