#define func_80192D14 func_80192D14_header_decl
#define RoomLib_ResetAndSignal_8019309C RoomLib_ResetAndSignal_8019309C_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_8019309C
#undef func_80192D14

extern int RoomLib_ResetAndSignal_8019309C(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80192D14
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_8019309C
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
