#include "pe1/field_engine_state.h"

int FieldEng_Spawn6(char *base, int unused, int index, int value) {
    base += 0xC;
    g_FieldEngineScriptState = base;
    *(int *)(base + index * 4 + 0x48) = value;
    return 0;
}
extern char *D_800E2248;

void *func_800C2B10(int index) {
    int offset;
    char *base;

    offset = index << 2;
    base = D_800E2248;
    offset += 8;
    return base + offset;
}
extern char *D_800E2248;

void *func_800C2B28(int index) {
    int offset;
    char *base;

    offset = index << 2;
    base = D_800E2248;
    offset += 0x48;
    return base + offset;
}

void func_800C2B40(void *context) {
    ((FieldEngState *)g_FieldEngineScriptState)->current_context = context;
}

void *func_800C2B50(void) {
    return ((FieldEngState *)g_FieldEngineScriptState)->current_context;
}

int func_800C2B68(void) {
    return ((*(unsigned int *)(g_FieldEngineScriptState + 4) & 0xFFFF0000U) ^ 0x01010000U) < 1;
}
