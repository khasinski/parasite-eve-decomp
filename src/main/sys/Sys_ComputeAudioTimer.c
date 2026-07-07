/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 */

extern int g_CombatModeFlags;
extern int D_800A76C8;
extern int D_800A76CC;
int Sys_ComputeAudioTimer(int **arg0)
{
  register int *dst;
  int a;
  int new_var;
  int b;
  int c;
  int value;
  dst = &D_800A76C8;
  a = (*arg0[0]) + (*arg0[1]);
  new_var = ((((((a * 8) - a) << 5) + a) * 16) - ((((a * 8) - a) << 5) + a)) << 6;
  value = new_var;
  c = *arg0[2];
  value += ((c << 4) - c) << 2;
  *dst = value;
  b = *arg0[3];
  if (b == 1)
  {
    D_800A76CC = 0;
    g_CombatModeFlags |= 2;
  }
  else
  {
    *dst = 0;
  }
  {
    int *flags;
    flags = &g_CombatModeFlags;
    *flags = ((*flags) | 1) & (~4);
  }
  return 1;
}
