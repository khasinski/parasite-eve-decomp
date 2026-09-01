#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_801914F4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191154
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914F4
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
