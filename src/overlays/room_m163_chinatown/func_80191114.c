/* MASPSX_FLAGS: --expand-div */
#define func_80191114 func_80191114_header_decl
#define RoomLib_ResetAndSignal_801914B4 RoomLib_ResetAndSignal_801914B4_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914B4
#undef func_80191114

extern int RoomLib_ResetAndSignal_801914B4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191114
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914B4
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
