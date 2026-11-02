#ifndef PE1_SCRIPT_QUEUE_H
#define PE1_SCRIPT_QUEUE_H

#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/script_node.h"

extern u32 *g_ScriptCursor;
extern u8 *D_8009D2F0[];
extern u8 *D_8009CDB0[];
extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern u32 g_GameStateFlags;
/* The 0x200 script variables (0x800 bytes), cleared with the entity pools
 * and saved in the save tail; operand mode 2 indexes them. */
extern u32 g_ScriptVariableBank[];

/* Script variables with a fixed role outside the scripts: the room word
 * (Boot_Shutdown saves the dispatch token there, the effect sequences set
 * 999 and pick their room by it), the scene flags word and the area word. */
#define SCRIPT_VAR_ROOM 1
#define SCRIPT_VAR_FLAGS 3
#define SCRIPT_VAR_AREA 0x4A
extern int (*g_ScriptQueueOpcodeHandlers[])(u32 **);

void Script_RunQueue(void);

#endif
