#ifndef MENU_MEMCARD_LINK_H
#define MENU_MEMCARD_LINK_H

#include "pe1/gte.h"
#include "menu_memcard_pulse.h"

typedef struct MemcardLinkOffset { u8 bytes[8]; } MemcardLinkOffset;
typedef struct MemcardLinkColor { u8 bytes[4]; } MemcardLinkColor;
typedef struct MemcardLinkState { s16 phase, timer, opacity, reserved06; GteRotation rotation; } MemcardLinkState;
typedef struct MemcardLinkChannel { s32 reserved[2]; void *slots; } MemcardLinkChannel;
extern MemcardLinkOffset D_801ED818;
extern MemcardLinkColor D_801ED850, D_801ED854;
extern void *D_8009D254;
extern MemcardLinkChannel *D_800F33E0;
extern s16 D_801F1D86, D_801F1D88;
extern u16 D_800E11E6, D_800E2850[];
extern void func_800CE8F0(void *, s32, void *, void *);
extern void func_800D1384(void *, void *, s32, void *, void *, s32, void *, s32);
extern s32 func_800CE560(void *, s32, s32, s32 (*)(s32, MemcardPulseState *));

s32 Memcard_LinkEffect(s32 mode, MemcardLinkState *state);

#endif
