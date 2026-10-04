#ifndef MENU_MEMCARD_VIDEO_H
#define MENU_MEMCARD_VIDEO_H

#include "common.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/psyq_cd.h"

typedef struct VideoFrame { u32 reserved[2], time, reserved0c; u16 width, height; } VideoFrame;
/* One entry of a video table: file name, colour depth and playback layout. */
typedef struct VideoEntry {
    char *name;            /* 0x00 file name appended to the directory */
    u8 wide;               /* 0x04 24-bit frames */
    s16 speed;             /* 0x06 */
    s16 end;               /* 0x08 last frame */
    u16 x, y;              /* 0x0A display origin */
    u8 reserved0e[6];
} VideoEntry;
typedef struct VideoRect { s16 x,y,w,h; } VideoRect;
/* Playback state of one video player copy. */
typedef struct VideoDisplay {
    u8 *decode[2];        /* 0x00 MDEC decode buffers */
    u8 decodeIndex;       /* 0x08 */
    u8 *buffers[2];       /* 0x0C slice upload buffers */
    u8 selector;          /* 0x14 */
    VideoRect regions[2]; /* 0x16 display areas of the two frame buffers */
    u8 region;            /* 0x26 */
    VideoRect rect;       /* 0x28 next slice */
    u8 done;              /* 0x30 frame fully uploaded */
} VideoDisplay;
extern s32 func_8007F72C(void),func_8007F7A8(void);
extern void func_800719E4(s32),func_800870F0(s32),func_80074F44(VideoRect *,s32,s32,s32);
extern s32 func_8007C484(void **,VideoFrame **);
extern u16 D_800B0DD4;
extern VideoEntry *D_801D11AC;
extern u8 D_800B0DBE,D_801D0DBD;
extern s8 D_800B0DBB;
extern s16 D_801D11B0,D_801D0DE0[2];

void *Memcard_UpdateVideoFrame(VideoDisplay *display);
void *func_80121270(VideoDisplay *display);

/* State of the first video player copy (linked at 0x80120D00). */
extern VideoEntry *D_801227E4;
extern s16 D_801227E8, D_80122418[2];
extern u8 D_801223F5;

extern u8 D_801D0DC0;
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
typedef struct VideoFrameCount { s16 value; } VideoFrameCount;
extern VideoFrameCount D_800B0DBC;
extern u32 D_800B0CD8;
extern void func_8007512C(VideoRect *,s32,s32);

s32 Memcard_InitVideoBuffers(s8 count, u8 **buffers);
s32 func_801216C4(s8 count, u8 **buffers);
/* Buffers and saved environments of the first video player copy. */
extern u8 *D_80122420, *D_80122424, *D_80122428, *D_8012242C, *D_80122430, *D_80122434;
extern VideoDisplayTemplate D_801227EC[2];
extern VideoDrawTemplate D_80122814[2];

typedef struct VideoDiscRange { u16 start,end; } VideoDiscRange;
extern VideoEntry D_801D0E00[];
extern VideoDiscRange D_8009315E;
extern u16 D_80093160,D_80093162;
extern u32 D_800B0DD8,D_8009D26C;
extern s32 func_8006E6A8(u32,void *,s32),CdRom_PollReady(void);
s32 Memcard_StepVideo(void);
extern void SetDispMask(s32),DrawSync(s32),func_80074A44(s32),func_80072714(void),func_800726C4(void),func_80072724(void),func_8003EB04(void),func_8010C0D8(void (*)(void)),func_8007A2A4(void),func_80080DC4(s32,s32,s32),Gpu_RenderFrame(void);
extern s32 VSync(s32);

s32 Memcard_PlayVideo(s32 index);

void Memcard_SetVideoDisplay(s8 index, s8 wide);
void func_80121004(s8 index, s8 wide);

/* Video open: file lookup, stream start and first decoded frame. */
typedef struct VideoFile { CdlLOC pos; u32 size; char name[16]; } VideoFile;
extern u8 D_800B0DBF;
extern VideoEntry D_80122438[];
extern VideoFile D_801223FC, D_801D0DC4;
extern CdlLOC D_80122414, D_801D0DDC;
extern VideoDisplay D_801228CC, D_801D1464;
extern char D_80120FF4[], D_80120FFC[], D_8018F2E4[], D_8018F2EC[];
extern void func_800719F4(char *, char *), func_8007A214(void *, s32);
extern void func_8007C304(s32, s32, s32, s32, s32), func_8007C394(void *);
extern void func_8010C89C(void *, void *, void *), func_801214D4(void);
/* DsSearchFile: the file record, 0 when absent, or all ones on a read error. */
extern u32 func_80081414(VideoFile *, char *); extern s32 func_80081314(CdlLOC *, s32);
extern s32 func_80080D5C(s32, CdlLOC *, void *);
s32 func_80121C04(s32 number);
s32 Memcard_OpenVideo(s32 number);

/* Per-frame video step: decode, restart the stream on a stall, page flip. */
extern u8 D_801223F6, D_801223F8, D_801D0DBE;
extern void func_8010BFA0(void *, s32), func_8007C2A0(CdlLOC *);
s32 func_80122040(void);


#endif
