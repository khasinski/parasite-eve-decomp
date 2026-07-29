#ifndef PE1_RENDER_CAMERA_H
#define PE1_RENDER_CAMERA_H

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

#endif
