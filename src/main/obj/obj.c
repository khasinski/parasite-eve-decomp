#include "pe1/geom_state.h"

s16 Obj_GetEntryField6(int index) {
    return ((GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4)))->field6;
}

int Obj_SetEntryField8(int index, unsigned int value) {
    ((GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4)))->field8 = value >> 8;
    return 0;
}
