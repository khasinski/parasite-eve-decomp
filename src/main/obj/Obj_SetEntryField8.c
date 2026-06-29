
#include "pe1/geom_state.h"

int Obj_SetEntryField8(int index, unsigned int value) {
    ((GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4)))->field8 = value >> 8;
    return 0;
}
