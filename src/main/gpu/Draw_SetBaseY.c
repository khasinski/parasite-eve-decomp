
typedef unsigned char u8;
typedef unsigned short u16;
extern u16 g_RenderDrawBaseY1;
extern u16 g_RenderDrawBaseY0;
extern u8 g_DrawBaseY;
int Draw_SetBaseY(int arg0, unsigned int arg1)
{
  int new_var;
  new_var = arg1 < 0x10;
  if (new_var)
  {
    register int value;
    value = arg1;
    g_DrawBaseY = (g_RenderDrawBaseY0 = (g_RenderDrawBaseY1 = value));
  }
  return 0;
}
