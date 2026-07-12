#include "../room_lib/room_lib.h"

extern char D_80190F4C[];

int func_8018F010(void) {
    *FieldEng_GetSlot() = D_80190F4C;
    return 0;
}
