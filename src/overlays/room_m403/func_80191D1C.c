#define func_80191D1C func_80191D1C_header_decl
#include "../room_lib/room_lib.h"
#undef func_80191D1C

extern int RoomLib_ResetAndSignal_801920A4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D1C
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920A4
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
