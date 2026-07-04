
extern signed char g_GameState[];
int Overlay_GetAudioSlotByKey(int arg0)
{
  register signed char *entry;
  int i;
  signed char *new_var;
  i = 0;
  entry = g_GameState;
  while (i < 2)
  {
    if (entry[0xDC] == arg0)
    {
      new_var = entry;
      entry = &new_var[0xDD];
      return *entry;
    }
    i++;
    entry += 2;
  }

  return -1;
}
