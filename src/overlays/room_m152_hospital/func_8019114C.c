/* MASPSX_FLAGS: --expand-div */
#define func_8019114C func_8019114C_header_decl
#define RoomLib_ResetAndSignal_801914EC RoomLib_ResetAndSignal_801914EC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914EC
#undef func_8019114C

extern int RoomLib_ResetAndSignal_801914EC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_8019114C
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914EC
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
