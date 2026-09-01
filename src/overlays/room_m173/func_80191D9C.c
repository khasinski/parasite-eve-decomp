#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_8019213C(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191D9C
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_8019213C
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
