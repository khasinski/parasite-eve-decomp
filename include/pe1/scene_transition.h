#ifndef PE1_SCENE_TRANSITION_H
#define PE1_SCENE_TRANSITION_H

/* Screen / room transitions (walking to an exit -> load the next pre-rendered
 * field screen: background, collision mesh, actors). Mapped via DuckStation GDB
 * snapshot-diff across transitions plus code; confirmed live.
 *
 * CURRENT ROOM ID:
 *   D_8009D280  current room id (32-bit, packed) -- the argument to
 *               Scene_LoadRoom. Reverts per room (live: room A=0xA8000148,
 *               room B=0xA80001C8, back to A=0xA8000148). The high half
 *               (0xA8000000) appears to be area/disc flags; the low half is the
 *               room within the area.
 *   D_8009D1C4  mirror of the current room id (same value).
 *   NOTE: D_8009CDDC is NOT the room id (a transient transition flag/counter
 *         that did not revert on return; ignore it for room identity).
 *
 * LOAD PATH:
 *   Gpu_InitPipeline()           -> Scene_LoadRoom(D_8009D280)
 *   Scene_LoadRoom(s32 roomId):
 *     Str_EncodeBase32(buf, roomId)  -> 6-char room name (table D_800930B4)
 *     Str_ParseMapNumber(buf)        -> 3-digit map number from name[2..4]
 *     CdRom_ReadSectorsFromLba(...)  -> stream room data from PE.IMG
 *                                       (base g_PeImageBaseLba)
 *   Other room entries: Scene_LoadMap (0x80035038), Scene_LoadSceneData
 *   (0x8006A9E4), Scene_LoadRoomAssets (per-entity textures/effects).
 *
 * PER-ROOM DATA reloaded on every transition (all confirmed reverting in the
 * snapshot-diff A->B->A):
 *   D_8009D1FC  collision DB pointer        (see field_collision.h)
 *   D_8009CE08  region-height table
 *   D_8009CE00 / D_8009CE04 / D_8009CE14 / D_8009CE90  scene-data table pointers
 *   g_PlayerEntity  player actor (re-allocated each room)
 *   0x8009D300.. scene task/entity node cluster (rebuilt)
 * Returning to a room reloads its data deterministically to the SAME addresses.
 *
 * Quick live probe: current room id = *(u32*)0x8009D280.
 *
 * Functions / globals:
 *   s32  Scene_LoadRoom(s32 roomId);         // 0x8006B4F8
 *   s32  Scene_LoadMap(...);                  // 0x80035038
 *   s32  Scene_LoadSceneData(...);            // 0x8006A9E4
 *   int  Str_EncodeBase32(char *out, u32 v);  // room id -> 6-char name
 *   int  Str_ParseMapNumber(char *name);      // 0x8006E454; name -> map number
 *   void Gpu_InitPipeline(void);              // calls Scene_LoadRoom(D_8009D280)
 *   g_PeImageBaseLba   base LBA of PE.IMG (room data source)
 *   g_MapIdTable = 0x800917E4
 */

#endif /* PE1_SCENE_TRANSITION_H */
