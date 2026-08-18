#ifndef PE1_FIELD_ENGINE_STATE_H
#define PE1_FIELD_ENGINE_STATE_H

/* One dispatch slot in g_FieldEngineSlotTable (0x40 entries, 6 bytes each).
 * The field engine walks the table every frame: flag==1 dispatches the
 * handler_id'th handler, flag==2 marks the slot for teardown. */
typedef struct FieldEngSlot {
    /* 0x0 */ unsigned char handler_id;  /* index into the handler table */
    /* 0x1 */ signed char flag;          /* 1 = active/dispatch, 2 = pending cleanup */
    /* 0x2 */ unsigned short counter;    /* dispatch count while active */
    /* 0x4 */ short data_offset;         /* byte offset into g_FieldEngineScriptData */
} FieldEngSlot;

/* Script-state work area (obj+0xC, pointed at by g_FieldEngineScriptState).
 * Mostly reached through the func_800C2B10/2B28 index accessors (int arrays at
 * +0x08 and +0x48); only a few header fields are touched by absolute offset
 * (e.g. an int at +0x70 get/set by func_800C2B50/2B40, still unnamed). */
typedef struct FieldEngState {
    /* 0x00 */ unsigned char pad_00[0x40];
    /* 0x40 */ int abort_flag;          /* set to 1 to abort the current frame (func_800C2B90) */
} FieldEngState;

extern char *g_FieldEngineScriptState __asm__("D_800E2248");
extern char *g_FieldEngineScriptObject __asm__("D_800F32A8");
extern char *g_FieldEngineScriptData __asm__("D_800F3330");
extern int *g_FieldEngineCommandTable __asm__("D_800F33B0");
extern unsigned char *g_FieldEngineSlotTable __asm__("D_800F34F4");

#endif /* PE1_FIELD_ENGINE_STATE_H */
