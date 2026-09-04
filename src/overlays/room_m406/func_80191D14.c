#define func_80191D14 func_80191D14_header_decl
#define RoomLib_ResetAndSignal_8019209C RoomLib_ResetAndSignal_8019209C_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_8019209C
#undef func_80191D14

extern int RoomLib_ResetAndSignal_8019209C(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D14
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_8019209C
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
