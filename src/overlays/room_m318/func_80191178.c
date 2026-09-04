#define func_80191178 func_80191178_header_decl
#define RoomLib_ResetAndSignal_80191518 RoomLib_ResetAndSignal_80191518_header_decl
#include "../room_lib/room_lib.h"
#undef RoomLib_ResetAndSignal_80191518
#undef func_80191178

extern int RoomLib_ResetAndSignal_80191518(RoomEnt *obj);

#define ROOMLIB_ADVANCE_ARC_NAME func_80191178
#define ROOMLIB_ADVANCE_ARC_RESET RoomLib_ResetAndSignal_80191518
#include "../room_lib/RoomLib_AdvanceArcToTarget.inc"
