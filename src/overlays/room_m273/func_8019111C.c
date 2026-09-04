#define func_8019111C func_8019111C_header_decl
#define RoomLib_ResetAndSignal_801914BC RoomLib_ResetAndSignal_801914BC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914BC
#undef func_8019111C

extern int RoomLib_ResetAndSignal_801914BC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_8019111C
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914BC
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
