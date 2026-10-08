#ifndef PE1_GPU_COMMAND_BUILDERS_H
#define PE1_GPU_COMMAND_BUILDERS_H

/* Little-endian RECT view used by the texture-window builder. Only the low
 * byte of each origin participates in the GPU's eight-pixel window units. */
typedef struct GpuTextureWindowRectBytes {
    unsigned char xLow, xHigh;
    unsigned char yLow, yHigh;
    short w, h;
} GpuTextureWindowRectBytes;
typedef char gpu_texture_window_rect_size[
    (sizeof(GpuTextureWindowRectBytes) == 8) ? 1 : -1];

int Gpu_BuildDrawAreaTopLeftCmd(int x, int y);
int Gpu_BuildDrawAreaBottomRightCmd(int x, int y);
unsigned int Gpu_BuildDrawOffsetCmd(unsigned int x, unsigned int y);
int Gpu_BuildDrawModeCmd(int dfe, int dtd, int tpage);
unsigned int Gpu_BuildTexWindowCmd(GpuTextureWindowRectBytes *rect);

#endif
