/* MASPSX_FLAGS: --expand-div */
#define func_80192118 func_80192118_header_decl
#define RoomLib_ResetAndSignal_801924B8 RoomLib_ResetAndSignal_801924B8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801924B8
#undef func_80192118

extern int RoomLib_ResetAndSignal_801924B8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80192118
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801924B8
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
