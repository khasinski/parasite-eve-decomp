#include "common.h"

/* A draw-mode word pair followed by the sprite primitive it prefixes. */
typedef struct GpuModeSprtPacket {
    u32 tag;
    u32 mode;
    u8 sprt[1];
} GpuModeSprtPacket;

void SetDrawMode(void *p, int dfe, int dtd, int tpage);
void SetSprt(unsigned char *arg0);
int Gpu_AppendPacketPrimitive(void *arg0, void *arg1);
void exit(int code);

void Gpu_InitDrawModeSprtPacket(GpuModeSprtPacket *packet, int tpage) {
    u8 *prim = packet->sprt;

    SetDrawMode(packet, 0, 1, tpage);
    SetSprt(prim);
    if (Gpu_AppendPacketPrimitive(packet, prim) != 0) {
        exit(-1);
    }
}
