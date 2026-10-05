#ifndef PE1_RENDER_CAMERA_H
#define PE1_RENDER_CAMERA_H

#include "pe1/gte_types.h"

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
