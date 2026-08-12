#ifndef PE1_OVERLAY_MATH_H
#define PE1_OVERLAY_MATH_H

int Overlay_Sin(int angle) __asm__("func_80077CF4");
int Overlay_Cos(int angle) __asm__("func_80077DC4");
void *Overlay_GetScriptActor(void) __asm__("func_800C2B50");

#endif
