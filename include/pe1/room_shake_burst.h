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

/* Sweep shake controller anchor: position plus the sweep angle handed to
 * each spawned particle. */
typedef struct RoomShakeSweepAnchor {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s32 angle;                    /* 0x08 */
} RoomShakeSweepAnchor;

typedef struct RoomShakeSweepParticle {
    s16 reserved00[3];
    s16 state;                    /* 0x06 */
    s16 reserved08;
    s16 angle;                    /* 0x0A */
    s16 timer;                    /* 0x0C */
} RoomShakeSweepParticle;

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
extern u16 D_800942EC;
extern u8 *D_800F32D8;
extern void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
extern void func_80020DD0(void);

/* room_m318 particle callbacks and the sweep anchor. */
extern int func_80193FC4(int mode, RoomShakeBurstPoint *particle);
extern int func_8019326C(int mode, RoomShakeSweepParticle *particle);
extern GteShortVector D_80199904;

#endif
