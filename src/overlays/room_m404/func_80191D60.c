#define func_80191D60 func_80191D60_header_decl
#define RoomLib_ResetAndSignal_801920E8 RoomLib_ResetAndSignal_801920E8_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801920E8
#undef func_80191D60

extern int RoomLib_ResetAndSignal_801920E8(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80191D60
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801920E8
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
