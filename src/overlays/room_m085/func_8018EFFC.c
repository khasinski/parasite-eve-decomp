#include "../room_lib/room_lib.h"

extern char D_80190040[];

int func_8018EFFC(void) {
    *FieldEng_GetSlot() = D_80190040;
    return 0;
}
