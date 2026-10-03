#ifndef FX_COMMON_SETUP_H
#define FX_COMMON_SETUP_H

#include "fx_common.h"

typedef struct FxCommonShortVec { s16 x, y, z, pad; } FxCommonShortVec;

typedef struct FxTextureSetup { s16 x,y,w,h,tpage,clut; s32 reserved; } FxTextureSetup;
typedef char FxTextureSetupSizeCheck[sizeof(FxTextureSetup)==16 ? 1 : -1];
typedef char FxTextureSetupTpageCheck[(u32)&((FxTextureSetup*)0)->tpage==8 ? 1 : -1];
typedef char FxTextureSetupClutCheck[(u32)&((FxTextureSetup*)0)->clut==10 ? 1 : -1];
extern FxTextureSetup g_FxTextureSetup[] asm("D_80091648");
extern u32 D_800A77FC;
extern s32 D_8019BFF4;
extern s32 D_8019BFF8;
extern s32 D_8019BFFC;
extern s32 D_8019C008;
extern s8 D_8019C00D;
extern u8 D_8019C017;
extern s32 D_8019C020;
extern s16 D_8019C02E;
extern s16 D_8019C034;
extern u8 D_8019C040;
extern s8 D_8019C041;
extern u8 D_8019C042;
extern u8 D_8019C044;
extern u8 D_8019C045;
extern u8 D_8019C046;
extern s32 D_8019C048;
extern short D_8019C0D4[];
extern short D_8019C0E8[];
extern unsigned char D_8019C0FC;
extern unsigned char D_8019C11C;
extern FxCommonNode *D_8019C148;
extern FxCommonNode *D_8019C14C;
extern FxCommonNode *D_8019C150;
extern FxCommonNode *D_8019C154;
extern FxCommonNode *D_8019C180;
extern FxCommonNode *D_8019C3B0[];
extern FxCommonNode *D_8019C820;
extern FxCommonNode *D_8019C824;
extern int D_8019CAAC[];
extern s32 D_8019CC1C;
extern FxCommonNode *D_8019CC20;
extern FxCommonNode *D_8019CC24;
extern FxCommonNode *D_8019CC28;
extern s16 D_8019CC52;
extern FxCommonNode *D_8019CDA0;
extern FxCommonNode *D_8019CDA4;
extern FxCommonNode *D_8019CDA8;
extern FxCommonNode *D_8019CDAC;
extern FxCommonNode *D_8019CDB0;
extern FxCommonNode *D_8019CDB4;
extern FxCommonNode *D_8019CDB8;
extern FxCommonNode *D_8019CDBC;
extern FxCommonNode *D_8019CDC0;
extern FxCommonNode *D_8019CDC4;
extern FxCommonNode *D_8019CDC8;
extern unsigned char D_8019CE10;
extern FxCommonNode *D_801EA578;
extern FxCommonNode *D_801EA57C;
extern FxCommonNode *D_801EA580;
extern FxCommonNode *D_801EA584;
extern FxCommonNode *D_801EA588;
extern FxCommonNode *D_801EA58C;
void func_800371B0(void *);
void func_80077E64(int,int,int);
void func_80078E34(void *);
void func_80078E64(void *);
void func_80078FC4(int,int,int);
void func_80078FE4(int,int,int);
void func_80190998(void);
FxCommonNode *func_80190AEC(FxCommonNode *,void *);
FxCommonNode *func_80190B78(FxCommonNode *,s16,void *);
FxCommonNode *func_80190C1C(FxCommonNode *,s16,s16,s16,void *,void *,void *);
void func_80191854(void);
void func_80195994(int,int,int,int);
void func_80195F6C(void);

extern FxCommonNode *g_FxCommonType26Nodes[4] __asm__("D_8019C15C");
extern FxCommonNode *g_FxCommonPairedNodes[2] __asm__("D_8019C9C8");
typedef struct FxCommonRuntime {
    FxCommonNode *node25;
    u32 unknown04;
    FxCommonIndexLink links[202];
    FxCommonNode *pairedNodes[20];
    FxCommonNode pool[200];
    FxCommonNode *node0;
} FxCommonRuntime;
typedef char FxRuntimeSizeCheck[(sizeof(FxCommonRuntime) == 0x57E4) ? 1 : -1];
typedef char FxRuntime_linksCheck[(((u32)&((FxCommonRuntime *)0)->links) == 0x8) ? 1 : -1];
typedef char FxRuntime_pairedNodesCheck[(((u32)&((FxCommonRuntime *)0)->pairedNodes) == 0x330) ? 1 : -1];
typedef char FxRuntime_poolCheck[(((u32)&((FxCommonRuntime *)0)->pool) == 0x380) ? 1 : -1];
typedef char FxRuntime_node0Check[(((u32)&((FxCommonRuntime *)0)->node0) == 0x57e0) ? 1 : -1];
extern FxCommonRuntime g_FxCommonRuntime __asm__("D_801E4A80");

typedef struct FxCommonEffectSetupRecord {
    s16 position[3];
    s16 reserved06;
    u8 motionIndices[4];
    s32 value0c;
    s32 value10;
    s16 subtype;
    s16 type;
    s16 variant;
    s16 rotation;
    s16 parameter3c;
    s16 parameter38;
    s16 reserved20;
    u8 value22, value23, enabled, value25;
    s16 reserved26;
    s32 value28, value2c;
    s16 value30, reserved32;
} FxCommonEffectSetupRecord;
extern FxCommonEffectSetupRecord g_FxCommonEffectSetup[10] __asm__("D_801EA370");

typedef struct FxCommonPoint8 { s16 x,y,z,pad; } FxCommonPoint8;
typedef struct FxCommonPointList { u8 header[6]; s16 count; FxCommonPoint8 points[1]; } FxCommonPointList;

PE1_STATIC_ASSERT(sizeof(FxCommonEffectSetupRecord) == 52, fx_setup_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FxCommonEffectSetupRecord, subtype) == 0x14, fx_setup_subtype);
PE1_STATIC_ASSERT(sizeof(FxCommonPoint8) == 8, fx_point_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FxCommonPointList, points) == 8, fx_point_list_header);

#endif
