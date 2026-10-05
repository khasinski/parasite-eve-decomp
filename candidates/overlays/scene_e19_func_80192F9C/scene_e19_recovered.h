#ifndef SCENE_E19_RECOVERED_H
#define SCENE_E19_RECOVERED_H
struct RenderColor;
#include "pe1/render_object.h"
typedef struct SceneE19RecoveredState {
    GteShortVector position;
    GteShortVector endpoint;
    GteShortVector origin;
    s16 state, timer;
} SceneE19RecoveredState;
int func_80192F9C(int mode, SceneE19RecoveredState *effect);
void *func_8006E498(void *base, u32 key);
void func_8006DF50(void *,int,int,int,int);
void func_800C6D5C(void *,u8,u8);
int func_80071A54(void);
u16 func_80077A64(int,int,int,int);
u16 func_80077AA4(int,int);
int func_80077CF4(int);
int func_80077DC4(int);
void func_80078CC4(GteMatrix *, GteVector *);
void func_80079754(GteShortVector *, GteMatrix *);
int func_800C6B90(GteShortVector *,int);
void func_800C6EC0(int,int);
void func_800C6ED8(int);
void func_800C6EF8(void *);
void func_800C6F4C(void *);
void func_800C6FA0(void *,u16);
void func_800C71E4(void *,GteMatrix *);
int func_800CE560(void *,int,int,void *);
void *func_800CE610(void *);
void func_800D1AE0(RenderColor *,int,int,int);
void func_800D1D24(int,int,int);
int func_800D3FD8(void);
extern GteShortVector D_8018F1D4;
extern RenderColor D_8018F1DC,D_8018F1E0;

typedef struct SceneE19RecoveredObject { u32 flags; } SceneE19RecoveredObject;
typedef struct SceneE19RecoveredChannel { int reserved[2]; SceneE19RecoveredObject **pool; } SceneE19RecoveredChannel;
extern SceneE19RecoveredChannel *D_800F32D0, *D_800F33E0;



#endif
