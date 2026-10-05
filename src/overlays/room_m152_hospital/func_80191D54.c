/* MASPSX_FLAGS: --expand-div */
#define func_80191D54 func_80191D54_header_decl
#define RoomLib_ResetAndSignal_801920DC RoomLib_ResetAndSignal_801920DC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801920DC
#undef func_80191D54

extern int RoomLib_ResetAndSignal_801920DC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D54
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920DC
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
