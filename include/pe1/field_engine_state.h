#ifndef PE1_FIELD_ENGINE_STATE_H
#define PE1_FIELD_ENGINE_STATE_H

#include "pe1/field_script_context.h"

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
 * func_800C2758 interprets the command table: each 32-bit command carries an
 * opcode in the high half and an operand in the low half. The int register
 * file at +0x08 and the variable file at +0x48 are also reached through the
 * func_800C2B10/2B28 index accessors; the active context pointer at +0x70 is
 * shared by func_800C2B50/2B40. */
typedef struct FieldEngState {
    /* 0x00 */ short wait;              /* frames to sleep before the next command */
    /* 0x02 */ short pc;                /* index into the command table */
    /* 0x04 */ unsigned char pad_04[2];
    /* 0x06 */ signed char keep_alive;  /* halted scripts report -1 unless set */
    /* 0x07 */ signed char halted;      /* set by the -1 end command */
    /* 0x08 */ int regs[16];            /* regs[14] = abort flag (func_800C2B90) */
    /* 0x48 */ int vars[9];
    /* 0x6C */ int *commands;           /* command table (obj+0x78) */
    /* 0x70 */ void *current_context;   /* active field object or its effect context */
} FieldEngState;

/* Field engine object: header, script state, dispatch slots, slot data. */
typedef struct FieldEngObject {
    /* 0x000 */ unsigned char header[0xC];
    /* 0x00C */ FieldEngState state;
    /* 0x080 */ FieldEngSlot slots[0x40];
    /* 0x200 */ unsigned char data[1];
} FieldEngObject;

extern char *g_FieldEngineScriptState __asm__("D_800E2248");
extern char *g_FieldEngineScriptObject __asm__("D_800F32A8");
extern char *g_FieldEngineScriptData __asm__("D_800F3330");
extern int *g_FieldEngineCommandTable __asm__("D_800F33B0");
extern unsigned char *g_FieldEngineSlotTable __asm__("D_800F34F4");

void **FieldEng_GetSlot(char *object);

/* Shared XYZ origin consumed by field effect initializers. */
struct FieldAnimPointTriple;
extern struct FieldAnimPointTriple D_800E2290;

#endif /* PE1_FIELD_ENGINE_STATE_H */
