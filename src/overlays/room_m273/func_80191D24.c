#define func_80191D24 func_80191D24_header_decl
#define RoomLib_ResetAndSignal_801920AC RoomLib_ResetAndSignal_801920AC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801920AC
#undef func_80191D24

extern int RoomLib_ResetAndSignal_801920AC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D24
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920AC
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
