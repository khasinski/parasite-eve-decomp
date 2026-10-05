
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef struct VideoFrame
{
  u32 reserved[2];
  u32 time;
  u32 reserved0c;
  u16 width;
  u16 height;
} VideoFrame;
typedef struct VideoEntry
{
  u8 reserved[8];
  s16 end;
} VideoEntry;
typedef struct VideoDisplay
{
  u8 reserved[26];
  s16 width1a;
  s16 height1c;
  u8 reserved1e[4];
  s16 width22;
  s16 height24;
  u8 reserved26[8];
  s16 height2e;
} VideoDisplay;
typedef struct VideoRect
{
  s16 x;
  s16 y;
  s16 w;
  s16 h;
} VideoRect;
extern s32 func_8007F72C(void);
extern s32 func_8007F7A8(void);
extern void func_800719E4(s32);
extern void func_800870F0(s32);
extern void func_80074F44(VideoRect *, s32, s32, s32);
extern s32 func_8007C484(void **, VideoFrame **);
extern u16 D_800B0DD4;
extern VideoEntry *D_801D11AC;
extern u8 D_800B0DBE;
extern u8 D_801D0DBD;
extern s8 D_800B0DBB;
extern s16 D_801D11B0;
extern s16 D_801D0DE0[2];
void *Memcard_UpdateVideoFrame(VideoDisplay *display);
extern s16 D_801D148C;
extern s16 D_801D148E;
extern s16 D_801D1490;
extern s16 D_801D1492;
extern VideoRect D_801D147A[2];
extern volatile u8 D_801D1478;
extern u8 D_801D148A;
extern u8 D_801D1494;
extern u8 D_801D0DC0;
extern void *D_801D1470[2];
extern s16 D_800B0CD0;
extern s32 D_8009CDDC;
extern void func_8007C564(void);
extern void func_8010C01C(void *, s32);
extern void func_801918F8(s8, s8);
extern void func_8007506C(void *, void *);
void Memcard_UploadVideoSlice(void);
typedef struct VideoDisplayTemplate
{
  u8 bytes[20];
} VideoDisplayTemplate;
typedef struct VideoDrawTemplate
{
  u32 words[23];
} VideoDrawTemplate;
extern VideoDisplayTemplate D_801D1384[2];
extern VideoDisplayTemplate D_800BCE80[2];
extern VideoDrawTemplate D_801D13AC[2];
extern VideoDrawTemplate D_800BCDC8[2];
extern u8 *D_801D0DE8;
extern u8 *D_801D0DEC;
extern u8 *D_801D0DF0;
extern u8 *D_801D0DF4;
extern u8 *D_801D0DF8;
extern u8 *D_801D0DFC;
extern u8 D_800B0DBA;
extern s16 D_800B0DBC;
extern u32 D_800B0CD8;
extern void func_8007512C(VideoRect *, s32, s32);
s32 Memcard_InitVideoBuffers(s8 count, u8 **buffers);
typedef struct VideoDiscRange
{
  u16 start;
  u16 end;
} VideoDiscRange;
typedef struct VideoPlaybackEntry
{
  u8 reserved[4];
  u8 enabled;
  u8 reserved05[15];
} VideoPlaybackEntry;
extern VideoPlaybackEntry D_801D0E00[];
extern VideoDiscRange D_8009315E;
extern u16 D_80093160;
extern u16 D_80093162;
extern void *D_8001160C;
extern u8 *D_80011610;
extern u32 D_800B0DD8;
extern u32 D_8009D26C;
extern s32 func_8006E6A8(u32, void *, s32);
extern s32 CdRom_PollReady(void);
extern s32 func_80192934(void);
extern s32 func_801924F8(s16);
extern void SetDispMask(s32);
extern void DrawSync(s32);
extern void func_80074A44(s32);
extern void func_80072714(void);
extern void func_800726C4(void);
extern void func_80072724(void);
extern void func_8003EB04(void);
extern void func_8010C0D8(s32);
extern void func_8007A2A4(void);
extern void func_80080DC4(s32, s32, s32);
extern void VSync(s32);
extern void Gpu_RenderFrame(void);
s32 Memcard_PlayVideo(s32 index);
extern u8 D_801D0DBE;
extern s16 D_800BCE84[];
extern s16 D_800BCDCC[];
extern u8 D_800BCE91[];
extern u8 D_800BCDE0[];
extern u8 D_800BCDDE[];
extern u8 D_800BCDDF[];
extern u8 D_800BCDE1[];
extern u8 D_800BCDE2[];
extern u8 D_800BCDE3[];
extern void func_800749D8(void *, s32, s32, s32, s32);
extern void func_80074924(void *, s32, s32, s32, s32);
void Memcard_SetVideoDisplay(s8 index, s8 wide);
typedef struct VideoDiscPosition
{
  u8 minute;
  u8 second;
  u8 sector;
  u8 track;
} VideoDiscPosition;
typedef struct VideoDecodeState
{
  void *frames[2];
  u8 frameIndex;
  u8 reserved09[3];
  void *slices[2];
  u8 sliceIndex;
  u8 reserved15;
  VideoRect regions[2];
  u8 regionIndex;
  u8 reserved27;
  VideoRect current;
  u8 done;
  u8 reserved31[3];
} VideoDecodeState;
extern VideoDecodeState D_801D1464;
extern u8 D_801D146C;
extern u8 D_801D0DBE;
extern VideoDiscPosition D_801D0DC4;
extern VideoDiscPosition D_801D0DDC;
extern void func_8010BFA0(void *, s32);
extern void func_8010C89C(void *, void *, void *);
extern void func_8007C394(void *);
extern void func_8007C2A0(VideoDiscPosition *);
extern s32 CdRom_GetPendingReadCount(void);
extern s32 func_80081314(VideoDiscPosition *, s32);
extern void func_80080D5C(s32, VideoDiscPosition *, volatile s32 *);
s16 Memcard_StepVideo(void)
{
  register s32 ended asm("$19");
  volatile s32 wait;
  register s32 retries asm("$16");
  register void *frame asm("$17");
  register u8 *selector asm("$16");
  register void **buffers asm("$20");
  register void **firstBuffers asm("$17");
  register s32 one asm("$18");
  register u8 *displayState asm("$16");
  register VideoDecodeState *state asm("$4");
  register s32 region asm("$2");
  register s32 frames asm("$3");
  register s32 abortState asm("$2");
  register s32 nextRetry asm("$2");
  register s32 clearDone asm("$5");
  register u8 *active asm("$3");
  register s16 decodeResult asm("$2");
  if (D_800B0DBA >= 2)
  {
    ended = 0;
    displayState = &D_801D0DC0;
    if ((*displayState) == 2)
    {
      Memcard_SetVideoDisplay(D_8009CDDC ^ 1, D_800B0DBB);
      *displayState = 0;
    }
    D_801D0DDC = D_801D0DC4;
    selector = &D_801D146C;
    firstBuffers = (void **) (selector - 8);
    func_8010BFA0(firstBuffers[*selector], D_801D0DBE);
    buffers = firstBuffers;
    asm("" : "=r"(buffers) : "0"(buffers));
    one = 1;
    selector += D_801D1478 * 4;
    func_8010C01C(*((void **) (selector + 4)), (D_801D1490 * D_801D1492) / 2);
    retry:
    retries = 2000;

    do
    {
      frame = Memcard_UpdateVideoFrame((VideoDisplay *) (&D_801D1464));
      nextRetry = retries - 1;
      if (frame)
      {
        break;
      }
      retries = nextRetry;
      asm("" : "=r"(nextRetry) : "0"(nextRetry));
    }
    while ((s16) nextRetry);
    if (!frame)
    {
      decodeResult = -1;
    }
    else
    {
      frames = (u16) D_800B0DBC;
      region = D_801D146C;
      frames++;
      region ^= 1;
      D_801D146C = region;
      D_800B0DBC = frames;
      asm("" : "=r"(region) : "0"(region));
      func_8010C89C(frame, buffers[region], D_801D0DF8);
      func_8007C394(frame);
      decodeResult = 0;
    }
    asm("" : "=r"(decodeResult) : "0"(decodeResult));
    if (decodeResult == (-1))
    {
      func_8007C2A0(&D_801D0DDC);
      do
      {
        while (func_8007F72C() != one)
        {
        }

      }
      while (CdRom_GetPendingReadCount() || ((func_80080D5C(2, &D_801D0DDC, &wait), !func_80081314(&D_801D0DDC, 480))));
      goto retry;
    }
    wait = 0x800000;
    asm("" : : : "memory");
    state = &D_801D1464;
    if (!(*((volatile u8 *) (&D_801D1494))))
    {
      clearDone = 1;
      do
      {
        if (!(--wait))
        {
          region = D_801D148A;
          state->done = clearDone;
          region ^= 1;
          D_801D148A = region;
          asm("" : "=r"(region) : "0"(region));
          D_801D148C = state->regions[region].x;
          asm("" : : : "memory");
          D_801D148E = state->regions[region].y;
        }
        asm("" : : : "$3", "$6", "$7", "memory");
      }
      while (!(*((volatile u8 *) (&D_801D1494))));
    }
    D_801D1494 = 0;
    if (D_801D0DBD == 1)
    {
      ended = 1;
    }
    abortState = ended;
    asm("" : "=r"(abortState) : "0"(abortState));
    if (!abortState)
    {
      return 1;
    }
    active = &D_800B0DBA;
    (*active)--;
    func_8010C0D8(0);
    func_8007A2A4();
    func_80080DC4(9, 0, 0);
  }
  return 0;
}
