#ifndef PE1_RENDER_CAMERA_H
#define PE1_RENDER_CAMERA_H

#include "pe1/gte_types.h"

/* Partial draw-context view used by viewport and scroll helpers. */
typedef struct RenderCameraGeomState {
    unsigned char pad_00[0x1C];
    int bounds_offset;
} RenderCameraGeomState;

typedef struct RenderCameraBounds {
    unsigned char pad_00[0x28];
    signed short width;
    signed short height;
    signed short min_x;
    signed short max_x;
    signed short min_y;
    signed short max_y;
} RenderCameraBounds;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCameraGeomState, bounds_offset) == 0x1C,
                  render_camera_geom_bounds_offset);
PE1_STATIC_ASSERT(sizeof(RenderCameraGeomState) == 0x20,
                  render_camera_geom_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCameraBounds, min_x) == 0x2C,
                  render_camera_bounds_min_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCameraBounds, max_x) == 0x2E,
                  render_camera_bounds_max_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCameraBounds, min_y) == 0x30,
                  render_camera_bounds_min_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderCameraBounds, max_y) == 0x32,
                  render_camera_bounds_max_y_offset);
PE1_STATIC_ASSERT(sizeof(RenderCameraBounds) == 0x34,
                  render_camera_bounds_size);

extern GteMatrix D_800BD000;
extern s16 D_800BD002, D_800BD004, D_800BD006, D_800BD008, D_800BD00A;
extern s16 D_800BD00C, D_800BD00E, D_800BD010;
extern int D_800BD014, D_800BD018, D_800BD01C;
int Render_DrawSprite(void);

/* Camera initialization scratch: matrix words followed by projection distance. */
extern unsigned int D_800B89F8[];
extern volatile int D_800B0E40;
extern short D_800BCFB4, D_800BCFB6;
int Menu_InitGlobals(void *matrix, void *screen);
int Geo_RenderMeshList(void *buffer, void **end);
int func_800655D4(void);
void SetGeomScreen(int distance);
int Render_PrepareFrame(void);

#endif
