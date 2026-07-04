
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern char *g_AkaoCurTrack;
extern u32 g_AkaoVoiceUpdateFlags;
extern u16 g_AkaoTrack5ATransposeValue;
unsigned int SeqOp_SetTrack5AValue(void *ptr)
{
  u8 *pc;
  int value;
  register char *track;
  pc = *((u8 **) ptr);
  *((u8 **) ptr) = pc + 1;
  value = pc[0];
  if ((*((u16 *) (((char *) ptr) + 0x54))) == 0)
  {
    if ((value & 0xC0) != 0)
    {
      ;
      *((u16 *) (g_AkaoCurTrack + 0x5A)) = ((*((u16 *) (g_AkaoCurTrack + 0x5A))) + (value & 0x3F)) & 0x3F;
    }
    else
    {
      track = g_AkaoCurTrack;
      *((u16 *) (track + 0x5A)) = value;
    }
  }
  else
    if ((value & 0xC0) != 0)
  {
    int m = value & 0x3F;
    g_AkaoTrack5ATransposeValue = (g_AkaoTrack5ATransposeValue + m) & 0x3F;
  }
  else
  {
    g_AkaoTrack5ATransposeValue = value;
  }
  g_AkaoVoiceUpdateFlags |= 0x10;
}
