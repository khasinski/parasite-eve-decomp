#include "../room_lib/room_lib.h"

typedef struct {
    short x;
    short y;
    short z;
    short pad;
} Entry;

void func_8018FE9C(void *unused0, void *unused1, char *state) {
    char *obj = (char *)func_800C2B50();
    unsigned int i;
    Entry *entries = (Entry *)state;

    for (i = 0; i < 0x10; i++) {
        entries[i].x = RW32(obj, 0x18);
        entries[i].y = RW32(obj, 0x1C);
        entries[i].z = RW32(obj, 0x20);
    }

    RW16(state, 0x82) = 0x80;
    RW16(state, 0x80) = 0x800;
}
