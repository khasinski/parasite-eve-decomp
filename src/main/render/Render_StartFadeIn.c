
typedef unsigned char u8;
extern int g_RenderStateFlags;
extern u8 g_RenderFadeFrameCount;
extern u8 g_RenderFadeFrameCounter;
int Render_StartFadeIn(int arg0)
{
  int new_var;
  register int *flags;
  int value;
  flags = &g_RenderStateFlags;
  g_RenderFadeFrameCount = arg0;
  g_RenderFadeFrameCounter = 0;
  new_var = *flags;
  value = new_var;
  value &= ~0xC00;
  value |= 0x400;
  *flags = value;
  return 0;
}
