/* MASPSX_FLAGS: --expand-div */
#define func_80191D80 func_80191D80_header_decl
#define RoomLib_ResetAndSignal_80192108 RoomLib_ResetAndSignal_80192108_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80192108
#undef func_80191D80

extern int RoomLib_ResetAndSignal_80192108(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D80
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_80192108
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
