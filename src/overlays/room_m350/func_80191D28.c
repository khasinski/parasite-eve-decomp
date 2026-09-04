#define func_80191D28 func_80191D28_header_decl
#define RoomLib_ResetAndSignal_801920B0 RoomLib_ResetAndSignal_801920B0_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801920B0
#undef func_80191D28

extern int RoomLib_ResetAndSignal_801920B0(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D28
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920B0
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
