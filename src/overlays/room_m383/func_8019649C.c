/* MASPSX_FLAGS: --expand-div */
#define func_8019649C func_8019649C_header_decl
#define RoomLib_ResetAndSignal_80196824 RoomLib_ResetAndSignal_80196824_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80196824
#undef func_8019649C

extern int RoomLib_ResetAndSignal_80196824(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_8019649C
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80196824
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
