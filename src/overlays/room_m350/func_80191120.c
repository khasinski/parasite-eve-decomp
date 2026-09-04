#define func_80191120 func_80191120_header_decl
#define RoomLib_ResetAndSignal_801914C0 RoomLib_ResetAndSignal_801914C0_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914C0
#undef func_80191120

extern int RoomLib_ResetAndSignal_801914C0(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191120
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914C0
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
