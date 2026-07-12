#include "../room_lib/room_lib.h"

extern char *D_8009D254;
typedef struct Copy8 {
    int w[8];
} Copy8;

extern Copy8 D_80190028;
extern RoomLink *D_80190048;
extern Copy8 D_80190070;

void func_8018F24C(RoomEnt *o) {
    D_80190048 = o->link;
    D_80190028 = *(Copy8 *)D_80190048->p238;

    D_80190070 = *(Copy8 *)((char *)D_80190048->p238 + 0x620);

    if ((char *)o->link == D_8009D254) {
        D_80190070 = *(Copy8 *)D_80190048->p238;
    }
}
