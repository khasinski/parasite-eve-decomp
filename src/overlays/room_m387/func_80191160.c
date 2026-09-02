#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_80191500(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191160
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80191500
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
