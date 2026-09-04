#define func_80192104 func_80192104_header_decl
#define RoomLib_ResetAndSignal_801924A4 RoomLib_ResetAndSignal_801924A4_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801924A4
#undef func_80192104

extern int RoomLib_ResetAndSignal_801924A4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80192104
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801924A4
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
