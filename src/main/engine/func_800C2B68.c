#include "pe1/field_engine_state.h"

int func_800C2B68(void) {
    return ((*(unsigned int *)(g_FieldEngineScriptState + 4) & 0xFFFF0000U) ^ 0x01010000U) < 1;
}
