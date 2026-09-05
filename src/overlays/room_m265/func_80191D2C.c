/* MASPSX_FLAGS: --expand-div */
#define func_80191D2C func_80191D2C_header_decl
#define RoomLib_ResetAndSignal_801920B4 RoomLib_ResetAndSignal_801920B4_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801920B4
#undef func_80191D2C

extern int RoomLib_ResetAndSignal_801920B4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D2C
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920B4
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
