#include "../room_lib/room_lib.h"

extern void RoomLib_RearmOnMatch_80193C38(void);

ROOMLIB_INIT_TIMER_ARGS(func_80193B70, RoomLib_RearmOnMatch_80193C38, 0x08064F00)

int func_80193C08(void) {
    return 0;
}
