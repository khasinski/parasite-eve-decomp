#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_80192090(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D08
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80192090
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
