/* MASPSX_FLAGS: --expand-div */
#define func_80191D20 func_80191D20_header_decl
#define RoomLib_ResetAndSignal_801920A8 RoomLib_ResetAndSignal_801920A8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801920A8
#undef func_80191D20

extern int RoomLib_ResetAndSignal_801920A8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D20
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920A8
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
