#include "../room_lib/room_lib.h"

extern char *D_8009D254;
typedef struct Copy8 {
    int w[8];
} Copy8;

extern Copy8 D_80196FC8;
extern RoomLink *D_80197408;
extern Copy8 D_801974E0;

void func_8018F494(RoomEnt *o) {
    D_80197408 = o->link;
    D_80196FC8 = *(Copy8 *)D_80197408->p238;

    D_801974E0 = *(Copy8 *)((char *)D_80197408->p238 + 0x620);

    if ((char *)o->link == D_8009D254) {
        D_801974E0 = *(Copy8 *)D_80197408->p238;
    }
}
