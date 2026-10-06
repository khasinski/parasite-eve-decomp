#ifndef SCENE_E08_MODULES_H
#define SCENE_E08_MODULES_H

#include "pe1/room_module.h"

/* scene_e08 links four effect modules, each behind a class of seven
 * methods in class-table slot order (pe1/room_module.h). The first module
 * (fade layers and particle cluster) uses the shared class units and the
 * room's g_Room* lists, with its own registration and start methods
 * (SceneE08_ModuleStart.c), which act whatever the field engine's status.
 * The other three modules each have a class unit of their own with the
 * same methods over their own script and lists, named in the symbol file. */
int SceneE08_RegisterDrawList(void *o);
int SceneE08_StartModule(void *o);

/* Second module: emitter and fade slots, the player orb and its ring
 * sprites (SceneE08_OrbClass.c). */
extern int g_SceneE08OrbScript[];
extern RoomModuleHandler g_SceneE08OrbDrawList[];
extern RoomModuleHandler g_SceneE08OrbUpdateList[];
extern RoomModuleHandler g_SceneE08OrbInitList[];
extern int g_SceneE08OrbSpawnLayout[];
int SceneE08_OrbNop0(void);
int SceneE08_OrbPlantScript(void);
int SceneE08_OrbSpawn6(int a, int b, int c, int d, int e, int f);
int SceneE08_OrbRegister(void *o);
int SceneE08_OrbStart(void *o);
int SceneE08_OrbClose(struct RoomEnt *o);
int SceneE08_OrbNop6(void);

/* Third module: moving and particle slots, the glow model and the limb
 * beams (SceneE08_SlotsClass.c). */
extern int g_SceneE08SlotsScript[];
extern RoomModuleHandler g_SceneE08SlotsDrawList[];
extern RoomModuleHandler g_SceneE08SlotsUpdateList[];
extern RoomModuleHandler g_SceneE08SlotsInitList[];
extern int g_SceneE08SlotsSpawnLayout[];
int SceneE08_SlotsNop0(void);
int SceneE08_SlotsPlantScript(void);
int SceneE08_SlotsSpawn6(int a, int b, int c, int d, int e, int f);
int SceneE08_SlotsRegister(void *o);
int SceneE08_SlotsStart(void *o);
int SceneE08_SlotsClose(struct RoomEnt *o);
int SceneE08_SlotsNop6(void);

/* Fourth module: effect palettes, the layered sprite and the arm glow
 * (SceneE08_LayeredSpriteClass.c). */
extern int g_SceneE08LayeredSpriteScript[];
extern RoomModuleHandler g_SceneE08LayeredSpriteDrawList[];
extern RoomModuleHandler g_SceneE08LayeredSpriteUpdateList[];
extern RoomModuleHandler g_SceneE08LayeredSpriteInitList[];
extern int g_SceneE08LayeredSpriteSpawnLayout[];
int SceneE08_LayeredSpriteNop0(void);
int SceneE08_LayeredSpritePlantScript(void);
int SceneE08_LayeredSpriteSpawn6(int a, int b, int c, int d, int e, int f);
int SceneE08_LayeredSpriteRegister(void *o);
int SceneE08_LayeredSpriteStart(void *o);
int SceneE08_LayeredSpriteClose(struct RoomEnt *o);
int SceneE08_LayeredSpriteNop6(void);

#endif
