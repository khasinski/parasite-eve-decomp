#ifndef PE1_ASSET_TIM_H
#define PE1_ASSET_TIM_H

#include "pe1/psyq_tim.h"

/* Game-side TIM helpers (Asset_TimImage.c): upload a TIM's image and
 * optional CLUT to VRAM, and locate its rectangles and pixel data. */
int *Asset_LoadTimImage(TimFile *tim);
RECT *Asset_GetTimClutRect(TimFile *tim);
RECT *Asset_GetTimImageRect(TimFile *tim);
int *Asset_GetTimImagePixels(TimFile *tim);
int *Asset_GetTimClutPixels(TimFile *tim);

#endif
