#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_80192D2C(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_801929A4
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80192D2C
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
