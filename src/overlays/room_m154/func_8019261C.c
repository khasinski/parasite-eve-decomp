/* MASPSX_FLAGS: --expand-div */
#define func_8019261C func_8019261C_header_decl
#define RoomLib_ResetAndSignal_801929BC RoomLib_ResetAndSignal_801929BC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801929BC
#undef func_8019261C

extern int RoomLib_ResetAndSignal_801929BC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_8019261C
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801929BC
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
