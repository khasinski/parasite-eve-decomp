/* MASPSX_FLAGS: --expand-div */
#define func_80191118 func_80191118_header_decl
#define RoomLib_ResetAndSignal_801914B8 RoomLib_ResetAndSignal_801914B8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914B8
#undef func_80191118

extern int RoomLib_ResetAndSignal_801914B8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191118
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914B8
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
