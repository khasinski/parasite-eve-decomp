#include "pe1/field_engine_state.h"

int FieldEng_Spawn6(char *base, int unused, int index, int value) {
    base += 0xC;
    g_FieldEngineScriptState = base;
    *(int *)(base + index * 4 + 0x48) = value;
    return 0;
}
