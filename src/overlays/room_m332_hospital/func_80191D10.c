/* MASPSX_FLAGS: --expand-div */
#define func_80191D10 func_80191D10_header_decl
#define RoomLib_ResetAndSignal_80192098 RoomLib_ResetAndSignal_80192098_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80192098
#undef func_80191D10

extern int RoomLib_ResetAndSignal_80192098(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D10
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80192098
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
