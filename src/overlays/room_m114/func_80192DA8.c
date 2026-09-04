#define func_80192DA8 func_80192DA8_header_decl
#define RoomLib_ResetAndSignal_80193130 RoomLib_ResetAndSignal_80193130_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80193130
#undef func_80192DA8

extern int RoomLib_ResetAndSignal_80193130(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80192DA8
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80193130
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
