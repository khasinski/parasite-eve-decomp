#ifndef PE1_ROOM_MODULE_H
#define PE1_ROOM_MODULE_H

/* A room module's class. Every effect module a room links starts with a
 * class of seven methods that the room's class table lists in slot order:
 * a no-op, RoomLib_PlantScript, RoomLib_Spawn6, a registration method
 * (RoomLib_RegisterDrawList in most rooms), a start method
 * (RoomLib_RegisterPairedTables in most rooms), RoomLib_CloseTarget and a
 * second no-op. The methods that are the same code in every room are shared
 * units in src/overlays/room_lib/, one per function, because which of them a
 * room links varies: some rooms replace the registration or start method
 * with their own.
 *
 * The lists the methods hand to the field engine are the module's own data,
 * named in each room's symbol file. A room that links several modules names
 * the lists of the module whose methods are the shared units; the other
 * modules keep their own copies of the methods. */

struct RoomEnt;

typedef int (*RoomModuleHandler)();

/* Command program planted into the engine slot (opcode in the high half of
 * each word, -1 terminated). */
extern int g_RoomScript[];
/* Per-object handler lists, -1 terminated, one entry per object type. */
extern RoomModuleHandler g_RoomDrawList[];
extern RoomModuleHandler g_RoomUpdateList[];
extern RoomModuleHandler g_RoomInitList[];
/* Spawn layout read with g_RoomInitList (object type and placement). */
extern int g_RoomSpawnLayout[];

int RoomLib_PlantScript(void);
int RoomLib_Spawn6(int a, int b, int c, int d, int e, int f);
int RoomLib_RegisterDrawList(void *o);
int RoomLib_RegisterPairedTables(void *o);
int RoomLib_CloseTarget(struct RoomEnt *o);

/* Arguments the scene script hands the room's effects (the script's
 * RoomLib_SetArgs3 call); three words in each room's data. */
extern int g_RoomScriptArgs[3];

int *RoomLib_SetArgs3(int unused, int a, int b, int c);

/* The single argument other rooms take the same way (RoomLib_SetArg1). */
extern int g_RoomScriptArg;

int *RoomLib_SetArg1(int unused, int value);

#endif
