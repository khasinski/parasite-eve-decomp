
typedef unsigned short u16;
extern unsigned int g_AyaParasiteSpellFlags;
void *Aya_GetParasiteSpellUnlockTable(void);
void Menu_CreateParasiteSpellInfoPanel(int arg0);
int Aya_UnlockParasiteSpellById(int arg0)
{
  u16 *entry;
  int i;
  int found;
  unsigned int *flags;
  int new_var2;
  unsigned int *new_var;
  int bit;
  u16 *start;
  start = Aya_GetParasiteSpellUnlockTable();
  i = 0;
  entry = start;
  loop:
  if ((*entry) == arg0)
  {
    goto done;
  }

  i++;
  if (i < 20)
  {
    entry += 2;
    goto loop;
  }
  done:
  found = i < 20;

  new_var2 = found;
  if (new_var2)
  {
    new_var = &g_AyaParasiteSpellFlags;
    bit = 1;
    flags = new_var;
    *flags |= bit << i;
    Menu_CreateParasiteSpellInfoPanel(i);
  }
  return new_var2;
}
