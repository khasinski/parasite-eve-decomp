#define func_80193E68 func_80193E68_header_decl
#define RoomLib_ResetAndSignal_801941F0 RoomLib_ResetAndSignal_801941F0_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801941F0
#undef func_80193E68

extern int RoomLib_ResetAndSignal_801941F0(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80193E68
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801941F0
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
