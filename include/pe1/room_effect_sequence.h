#ifndef PE1_ROOM_EFFECT_SEQUENCE_H
#define PE1_ROOM_EFFECT_SEQUENCE_H

#include "common.h"

/* The effect sequence (src/overlays/room_lib/RoomFx_EffectSequence.c):
 * the room data it fills and reads (the same addresses in all five rooms
 * that link it) and the main code it calls. */

extern void *D_800B0E64;
extern u8 D_80192C08;
extern u8 D_80192C09;
extern u8 D_80192C0A;
extern u8 D_80192C0C;
extern u8 D_80192C0D;
extern u8 D_80192C0E;
extern s16 D_80192C10;
extern u8 D_80192C18;
extern u8 D_80192C19;
extern u8 D_80192C1A;
extern u8 D_80192C1C;
extern u8 D_80192C1D;
extern u8 D_80192C1E;
extern s16 D_80192C20;
extern s16 D_80192C22;
extern u8 D_80192C28;
extern u8 D_80192C29;
extern u8 D_80192C2A;
extern u8 D_80192C2C;
extern u8 D_80192C2D;
extern u8 D_80192C2E;
extern s16 D_80192C30;
extern s16 D_80192C32;
extern void *D_80192C34;
extern u8 D_80192C38;
extern u8 D_80192C39;
extern u8 D_80192C3A;
extern u8 D_80192C3C;
extern u8 D_80192C3D;
extern u8 D_80192C3E;
extern s16 D_80192C40;
extern s16 D_80192C42;
extern void *func_8006E498(void *base, int id);
extern int *func_800C2B28(int index);
extern int *func_800C2B10(int index);
extern int func_800C2B68(void);
extern void func_800C2B90(int owner, int command, void *config, void *state);
extern char D_80192B5C;
extern char D_80192BBC;
extern volatile s16 D_80192C12;
extern s16 D_800942EC;
extern char D_80192BFC;
extern char D_80192BF0;
extern short D_80192C30;
extern void func_800C2EAC(unsigned char);
extern void func_800C2FF0(int, int);
extern void func_800C3098(int);
extern void func_800C3238(int);
extern void func_80071A44(void *, int, int);

/* The module's class methods (slots 3 to 5). */
int RoomFx_EffectSequenceRegister(void *o);
int RoomFx_EffectSequenceStart(void *o);
s32 RoomFx_EffectSequenceNop5(void *o);

#endif
