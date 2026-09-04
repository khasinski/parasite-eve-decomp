#define func_80193740 func_80193740_header_decl
#define RoomLib_ResetAndSignal_80193AC8 RoomLib_ResetAndSignal_80193AC8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80193AC8
#undef func_80193740

extern int RoomLib_ResetAndSignal_80193AC8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80193740
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80193AC8
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
