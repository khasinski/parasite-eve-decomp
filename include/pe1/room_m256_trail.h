#ifndef PE1_ROOM_M256_TRAIL_H
#define PE1_ROOM_M256_TRAIL_H

#include "pe1/field_actor.h"
#include "pe1/field_anim.h"

/* Shared 24-byte pool record: steering trail, damped sparks, model burst,
 * expanding sprite and floor impact. Modes 1 and 2 update and draw it. */
typedef struct RoomM256TrailEffect {
    s16 x, y, z, index;               /* 0x00 */
    s16 motionX, motionY, motionZ;     /* 0x08: angles or particle velocity */
    s16 ending;                       /* 0x0E */
    s16 speed, angle, kind, frame;     /* 0x10 */
} RoomM256TrailEffect;
PE1_STATIC_ASSERT(sizeof(RoomM256TrailEffect) == 0x18, room_m256_trail_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM256TrailEffect, kind) == 0x14,
                  room_m256_trail_effect_kind);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM256TrailEffect, frame) == 0x16,
                  room_m256_trail_effect_frame);

extern FieldActor *D_8009D254;
extern RenderColor D_8018F218, D_8018F21C;
extern GteRotation D_8018F220,D_8018F1CC;
extern char D_80195EFC[],D_80195F1C[],D_80195F1E[],D_80195F20[];
extern s32 D_80195EF0,D_80195EF8,D_800E27EC,D_800F3428;
extern s16 D_800942EC,D_800F336A,D_800F336E,D_800F3372,D_800F3374,D_800F3376,D_800F3378;
extern u16 D_800E11EA,D_800E1204[],D_800E1208,D_800E2850[],D_800F336C,D_800F3370;
typedef struct RoomM256TrailPalettes { u16 first, second; } RoomM256TrailPalettes;
extern RoomM256TrailPalettes D_800E11E8;
int func_80071A54(void);
int func_80077A64(int,int,int,int);
int func_80077AA4(int,int);
int func_80077CF4(int);
int func_80077DC4(int);
void func_80078CC4(void *,void *);
void func_80079754(void *,void *);
int func_800C6B90(void *,int);
void func_800C6EC0(int,int);
void func_800C6ED8(int);
void func_800C6EF8(int);
void func_800C6F4C(int);
void func_800C6FA0(int,int);
void func_800C7098(int,int,int,int);
void func_800C71E4(int,void *);
void *func_800CE610(char *);
void func_800CE8F0(void *,int,void *,void *);
void func_800CFAA8(void *,void *,void *);
void func_800CFD50(void *,void *,int);
void func_800D3114(void *,int,int,int,int,int,int,int,int,int,void *,void *,int);
void func_800D3AFC(void *,int,void *,int);

int func_801940B0(int mode, void *effect);

#endif
