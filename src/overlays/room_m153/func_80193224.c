#define func_80193224 func_80193224_header_decl
#define RoomLib_ResetAndSignal_801935AC RoomLib_ResetAndSignal_801935AC_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801935AC
#undef func_80193224

extern int RoomLib_ResetAndSignal_801935AC(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_Y_NAME func_80193224
#define ROOMLIB_ADVANCE_ARC_Y_RESET RoomLib_ResetAndSignal_801935AC
#include "../room_lib/RoomLib_AdvanceArcToTargetY.inc"
