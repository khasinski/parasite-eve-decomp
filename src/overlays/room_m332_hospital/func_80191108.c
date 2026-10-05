/* MASPSX_FLAGS: --expand-div */
#define func_80191108 func_80191108_header_decl
#define RoomLib_ResetAndSignal_801914A8 RoomLib_ResetAndSignal_801914A8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914A8
#undef func_80191108

extern int RoomLib_ResetAndSignal_801914A8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191108
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914A8
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
