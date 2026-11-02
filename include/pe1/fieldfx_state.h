#ifndef PE1_FIELDFX_STATE_H
#define PE1_FIELDFX_STATE_H

#include "pe1/field_script_context.h"
#include "pe1/fieldfx_slot.h"
#include "pe1/fieldfx_task_pool.h"

/* Script-state work area (obj+0xC, pointed at by g_FieldFxScriptState).
 * FieldFx_RunScript interprets the command table: each 32-bit command
 * carries an opcode in the high half and an operand in the low half. The int
 * register file at +0x08 and the variable file at +0x48 are also reached
 * through FieldFx_RegPtr/FieldFx_VarPtr; the active context
 * pointer at +0x70 is shared by FieldFx_SetScriptContext and
 * FieldFx_GetScriptContext. */
typedef struct FieldFxState {
    /* 0x00 */ short wait;              /* frames to sleep before the next command */
    /* 0x02 */ short pc;                /* index into the command table */
    /* 0x04 */ short data_next;         /* next free slot-data offset */
    /* 0x06 */ signed char slot_count;  /* live dispatch slots; a halted script
                                         * reports -1 once it reaches zero */
    /* 0x07 */ signed char halted;      /* set by the -1 end command */
    /* 0x08 */ int regs[16];            /* regs[14] = slot table full (FieldFx_StartSlot) */
    /* 0x48 */ int vars[9];
    /* 0x6C */ int *commands;           /* command table (obj+0x78) */
    /* 0x70 */ void *current_context;   /* active field object or its effect context */
} FieldFxState;

/* Word view of the same work area: the halt tests compare slot_count and
 * halted (the high half of the word at +4) in one load. */
typedef struct FieldFxStateWords {
    /* 0x00 */ u32 timing;  /* wait and pc */
    /* 0x04 */ u32 status;  /* data_next, slot_count, halted */
} FieldFxStateWords;

/* Field engine object: header, script state, dispatch slots, slot data. */
typedef struct FieldFxObject {
    /* 0x000 */ FieldFxObjectPrefix prefix;
    /* 0x00C */ FieldFxState state;
    /* 0x080 */ FieldFxSlot slots[0x40];
    /* 0x200 */ char data[1];
} FieldFxObject;

/* Bytes of slot data after the dispatch slots (object size 0xA0C). */
#define FIELDFX_DATA_SIZE 0x80C

/* The object the engine is running and its parts, set by every pass. */
extern FieldFxState *g_FieldFxScriptState;
extern FieldFxObject *g_FieldFxScriptObject;
extern char *g_FieldFxScriptData;
extern int *g_FieldFxCommandTable;
extern FieldFxSlot *g_FieldFxSlotTable;

/* Messages printed for a slot whose handler list entry is -1. */
extern char g_FieldFxServiceThreadMissingMsg[];
extern char g_FieldFxUpdateThreadMissingMsg[];

/* Object passes (fieldfx/object_script.c). FieldFx_Register runs the
 * service (draw) handler of every active slot, FieldFx_UpdateSlots the
 * update handler and frees the slots marked for teardown, FieldFx_RunScript
 * steps the command table and starts slots from the init list with the slot
 * data sizes. FieldFx_Spawn6 stores a script variable; the effect classes
 * forward their six spawn arguments to it unchanged. */
int FieldFx_Register(FieldFxObject *obj, FieldFxHandler *handlers);
int FieldFx_UpdateSlots(FieldFxObject *obj, FieldFxHandler *handlers);
int FieldFx_RunScript(FieldFxObject *obj, FieldFxHandler *handlers, u16 *sizes);
int FieldFx_Spawn6();
int *FieldFx_RegPtr(int index);
int *FieldFx_VarPtr(int index);
void FieldFx_ActivateSlot(u16 slot, u8 id, u16 size);
int FieldFx_FreeSlot(u16 slot);
int FieldFx_FreeAllSlots(void);
/* Object status (fieldfx/collision.c); 3 = running. */
int FieldFx_GetStatus(void *obj);

/* Shared XYZ origin consumed by field effect initializers. */
struct FieldFxPointTriple;
extern struct FieldFxPointTriple g_PointSpriteActorPosition;

/* Shape helpers (fieldfx/geometry.c): the two CLUT pixel blocks
 * FieldFx_LoadEffectTexturePage uploads and the index of the loaded one, the colour-key table
 * and a word of the direction helpers. */
extern void *g_FieldEffectTexturePage0;
extern void *g_FieldEffectTexturePage1;
extern s16 g_FieldFxLoadedTexturePage;
extern u16 D_800E21A8[];
extern u16 D_800E1C68[];
extern u16 g_FieldFxRandomPhaseCounter;

#endif /* PE1_FIELDFX_STATE_H */
