#ifndef PE1_ROOM_M089_SPIN_MODEL_H
#define PE1_ROOM_M089_SPIN_MODEL_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/field_model_draw.h"

/* room_m089 spinning model: a glow sprite swells over the anchor while two
 * copies of a loaded model turn and shrink above it. */
typedef struct RoomM089SpinModel {
    /* 0x00 */ s16 timer;
    /* 0x02 */ s16 stage;
} RoomM089SpinModel;

extern RenderColor D_8018F1D4;
extern u8 *D_80194168;
/* The sound owner read as a one-field record keeps its load below the
 * state stores, as retail has it. */
typedef struct RoomM089SoundOwner {
    void *channel;
} RoomM089SoundOwner;

extern RoomM089SoundOwner D_800B0E64;
extern u16 D_800E11FA;
extern u16 D_800E120A;
u8 *func_8006E498(void *channel, int id);
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
void func_800C6FA0(u8 *data, u16 factor);
u16 GetClut(int x, int y);
int func_80077CF4(int angle);
int func_80077DC4(int angle);

#endif
