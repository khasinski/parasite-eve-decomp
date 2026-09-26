#ifndef MENU_MEMCARD_VIDEO_H
#define MENU_MEMCARD_VIDEO_H

#include "common.h"

typedef struct VideoFrame { u32 reserved[2], time, reserved0c; u16 width, height; } VideoFrame;
typedef struct VideoEntry { u8 reserved[8]; s16 end; } VideoEntry;
typedef struct VideoDisplay { u8 reserved[26]; s16 width1a, height1c; u8 reserved1e[4]; s16 width22, height24; u8 reserved26[8]; s16 height2e; } VideoDisplay;
typedef struct VideoRect { s16 x,y,w,h; } VideoRect;
extern s32 func_8007F72C(void),func_8007F7A8(void);
extern void func_800719E4(s32),func_800870F0(s32),func_80074F44(VideoRect *,s32,s32,s32);
extern s32 func_8007C484(void **,VideoFrame **);
extern u16 D_800B0DD4;
extern VideoEntry *D_801D11AC;
extern u8 D_800B0DBE,D_801D0DBD;
extern s8 D_800B0DBB;
extern s16 D_801D11B0,D_801D0DE0[2];

void *Memcard_UpdateVideoFrame(VideoDisplay *display);

extern s16 D_801D148C, D_801D148E;
extern s16 D_801D1490, D_801D1492;
extern VideoRect D_801D147A[2];
extern volatile u8 D_801D1478;
extern u8 D_801D148A,D_801D1494,D_801D0DC0;
extern void *D_801D1470[2];
extern s16 D_800B0CD0;
extern s32 D_8009CDDC;
extern void func_8007C564(void),func_8010C01C(void *,s32),func_801918F8(s8,s8);
extern void func_8007506C(void *,void *);

void Memcard_UploadVideoSlice(void);

typedef struct VideoDisplayTemplate { u8 bytes[20]; } VideoDisplayTemplate;
typedef struct VideoDrawTemplate { u32 words[23]; } VideoDrawTemplate;
extern VideoDisplayTemplate D_801D1384[2],D_800BCE80[2];
extern VideoDrawTemplate D_801D13AC[2],D_800BCDC8[2];
extern u8 *D_801D0DE8,*D_801D0DEC,*D_801D0DF0,*D_801D0DF4,*D_801D0DF8,*D_801D0DFC;
extern u8 D_800B0DBA;
extern s16 D_800B0DBC;
extern u32 D_800B0CD8;
extern void func_8007512C(VideoRect *,s32,s32);

s32 Memcard_InitVideoBuffers(s8 count, u8 **buffers);

typedef struct VideoDiscRange { u16 start,end; } VideoDiscRange;
typedef struct VideoPlaybackEntry { u8 reserved[4],enabled,reserved05[15]; } VideoPlaybackEntry;
extern VideoPlaybackEntry D_801D0E00[];
extern VideoDiscRange D_8009315E;
extern u16 D_80093160,D_80093162;
extern void *D_8001160C;
extern u8 *D_80011610;
extern u32 D_800B0DD8,D_8009D26C;
extern s32 func_8006E6A8(u32,void *,s32),CdRom_PollReady(void),func_80192934(void),func_801924F8(s16);
extern void SetDispMask(s32),DrawSync(s32),func_80074A44(s32),func_80072714(void),func_800726C4(void),func_80072724(void),func_8003EB04(void),func_8010C0D8(s32),func_8007A2A4(void),func_80080DC4(s32,s32,s32),Gpu_RenderFrame(void);
extern s32 VSync(s32);

s32 Memcard_PlayVideo(s32 index);

extern u8 D_801D0DBE;
extern s16 D_800BCE84[], D_800BCDCC[];
extern u8 D_800BCE91[], D_800BCDE0[], D_800BCDDE[], D_800BCDDF[], D_800BCDE1[], D_800BCDE2[], D_800BCDE3[];
extern void func_800749D8(void *,s32,s32,s32,s32),func_80074924(void *,s32,s32,s32,s32);

void Memcard_SetVideoDisplay(s8 index, s8 wide);

#endif
