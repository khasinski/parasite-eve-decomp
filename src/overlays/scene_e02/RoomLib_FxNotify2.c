#include "../room_lib/room_lib.h"

void RoomLib_FxNotify2(RoomLink *link, struct RoomSub *sub) {
    char *ent = (char *)link;
    char *rec = (char *)sub;
    if (*(short *)(rec + 0x6E) > 0) {
        int *dst = (int *)(rec + 0x50);
        char *v = *(char **)(rec + 0x64);
        if (v != 0) {
            dst[0] = *(int *)(v + 0x28);
            dst[2] = *(int *)(v + 0x30);
        }
        *(short *)(ent + 0x3A) = FieldEng_TurnToward(
            *(short *)(ent + 0x3A),
            (short)FieldEng_VecToAngle(dst, (int *)(ent + 0x28)),
            *(short *)(rec + 0x6E));
    }
}
