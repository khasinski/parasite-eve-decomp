#ifndef PE1_ROOM_DROP_FIELD_CLASS_H
#define PE1_ROOM_DROP_FIELD_CLASS_H

#include "common.h"
#include "pe1/room_module.h"

/* The drop field effect set's module class (RoomFx_DropFieldClass.c), in
 * class-table slot order, and the script and lists it hands the field
 * engine, named in each room's symbol file. The start method spawns from
 * the spawn data with the spawn script as the layout; the set's effects
 * (pe1/room_drop_field.h) spawn from the same two tables. */
extern int g_RoomDropFieldScript[];
extern RoomModuleHandler g_RoomDropFieldDrawList[];
extern RoomModuleHandler g_RoomDropFieldUpdateList[];
extern u8 g_RoomDropFieldSpawnScript[];
extern u8 g_RoomDropFieldSpawnData[];

int RoomFx_DropFieldNop0(void);
int RoomFx_DropFieldPlantScript(void);
int RoomFx_DropFieldSpawn6(int a, int b, int c, int d, int e, int f);
int RoomFx_DropFieldRegister(struct RoomEnt *o);
int RoomFx_DropFieldStart(void *o);
int RoomFx_DropFieldClose(struct RoomEnt *o);
int RoomFx_DropFieldNop6(void);

#endif
