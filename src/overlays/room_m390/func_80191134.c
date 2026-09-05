/* MASPSX_FLAGS: --expand-div */
#define func_80191134 func_80191134_header_decl
#define RoomLib_ResetAndSignal_801914D4 RoomLib_ResetAndSignal_801914D4_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_801914D4
#undef func_80191134

extern int RoomLib_ResetAndSignal_801914D4(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191134
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_801914D4
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
