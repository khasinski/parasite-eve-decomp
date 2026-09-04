#define func_80192E70 func_80192E70_header_decl
#define RoomLib_ResetAndSignal_801931F8 RoomLib_ResetAndSignal_801931F8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801931F8
#undef func_80192E70

extern int RoomLib_ResetAndSignal_801931F8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80192E70
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801931F8
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
