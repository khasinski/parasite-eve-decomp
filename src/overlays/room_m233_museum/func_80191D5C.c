/* MASPSX_FLAGS: --expand-div */
#include "../room_lib/room_lib.h"

extern int RoomLib_ResetAndSignal_801920E4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D5C
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920E4
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
