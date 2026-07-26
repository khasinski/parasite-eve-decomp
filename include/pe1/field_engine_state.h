#ifndef PE1_FIELD_ENGINE_STATE_H
#define PE1_FIELD_ENGINE_STATE_H

extern char *g_FieldEngineScriptState __asm__("D_800E2248");
extern char *g_FieldEngineScriptObject __asm__("D_800F32A8");
extern char *g_FieldEngineScriptData __asm__("D_800F3330");
extern int *g_FieldEngineCommandTable __asm__("D_800F33B0");
extern unsigned char *g_FieldEngineSlotTable __asm__("D_800F34F4");

#endif /* PE1_FIELD_ENGINE_STATE_H */
