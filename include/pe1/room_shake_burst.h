#ifndef PE1_ROOM_SHAKE_BURST_H
#define PE1_ROOM_SHAKE_BURST_H

#include "common.h"
#include "pe1/render_object.h"

/* Shake burst controller (room_m318): shakes the camera while it scatters
 * debris particles around its anchor, then flags the actor for the
 * scene script. */

typedef struct RoomShakeBurstPoint {
    s16 x, y, z;
} RoomShakeBurstPoint;

typedef struct RoomShakeBurstObject {
    u32 flags;                    /* 0x00 */
    u8 reserved04[0x14];
    u8 *status;                   /* 0x18 */
} RoomShakeBurstObject;

typedef struct RoomShakeBurstSlot {
    RoomShakeBurstObject *object;
} RoomShakeBurstSlot;

typedef struct RoomShakeBurstChannel {
    s32 reserved[2];
    RoomShakeBurstSlot *pool;     /* 0x08 */
} RoomShakeBurstChannel;

typedef struct RoomShakeBurstSparkChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} RoomShakeBurstSparkChannel;

typedef struct RoomShakeBurstEntity {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomShakeBurstEntity;

typedef struct RoomShakeBurstEvent {
    u8 reserved[0xD];
    u8 active;                    /* 0x0D */
} RoomShakeBurstEvent;

extern RoomShakeBurstEvent *D_800E2368;
extern RoomShakeBurstChannel *D_800F32D0;
extern RoomShakeBurstSparkChannel *D_800F33E0;
extern RoomShakeBurstEntity **D_8009D254;
extern void *D_800B0E64;
extern int D_800E27EC;
extern u16 D_800E11EA;
extern u16 D_800E2850[];

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomShakeBurstPoint *func_800CE610(void *pool);
extern int func_800D3FD8(void);
extern void func_8006DF50(void *channel, int id, int value, int volume, int pan);
extern int func_80071A54(void);
extern void func_80020D50(void);
extern void func_80020DD0(void);

#endif
