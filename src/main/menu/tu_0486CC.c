/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuBattleCount;

int g_MenuBattleCount;
short g_BattleCountTable[];

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef s32 M2C_UNK;
typedef s8 M2C_UNK8;
typedef s16 M2C_UNK16;
typedef s32 M2C_UNK32;
typedef s64 M2C_UNK64;
M2C_UNK Sfx_DrawSlotRow();
void *Item_LookupBaseData();
M2C_UNK *Str_LookupTable8();
extern u8 g_KeyItemDataTable[];
extern s16 g_BattleCountTable[];
extern u8 g_EquipItemDataTable[];
extern u8 g_CursorRenderDataBlock[];

int Menu_GetBattleCount(void) {
    return g_MenuBattleCount;
}

int Menu_GetBattleCountEntry(int arg0) {
    if (arg0 >= 0 && arg0 < g_MenuBattleCount) {
        return g_BattleCountTable[arg0];
    }
    return 0;
}

void Inv_ShowItemDescriptionPanel(s32 arg0)
{
  int new_var;
  M2C_UNK *var_a1;
  M2C_UNK *new_var3;
  s16 temp_v1;
  u32 temp_a0;
  s16 *new_var2;
  void *temp_s0;
  void *var_a0;
  void *var_s0;
  new_var2 = (&g_BattleCountTable[0]) + arg0;
  temp_v1 = *new_var2;
  if (temp_v1 < 0x200)
  {
    temp_a0 = temp_v1 - 1;
    new_var = ((u32) (temp_v1 - 0x100)) < 0x80U;
    if (new_var)
    {
      var_s0 = (temp_v1 << 5) + (&g_EquipItemDataTable[0]);
    }
    else
      if (temp_a0 < 0xFFU)
    {
      var_s0 = Item_LookupBaseData(temp_a0);
    }
    else
      if (((u32) (temp_v1 - 0x200)) < 9U)
    {
      var_s0 = (temp_v1 << 5) + (&g_KeyItemDataTable[0]);
    }
    else
    {
      var_s0 = (void *) 0;
    }
    if (var_s0 != ((void *) 0))
    {
      new_var3 = Str_LookupTable8((*((u8 *) (((s8 *) var_s0) + 4))) - 1);
      var_a0 = var_s0;
      var_a1 = new_var3;
      goto block_14;
    }
  }
  else
  {
    temp_s0 = (temp_v1 << 5) + (&g_KeyItemDataTable[0]);
    if ((*((u8 *) (((s8 *) temp_s0) + 5))) & 0x10)
    {
      var_a1 = &g_CursorRenderDataBlock[0];
      var_a0 = temp_s0;
      if ((*((u8 *) (6 + ((s8 *) temp_s0)))) == 9)
      {
        var_a1 = (&g_CursorRenderDataBlock[0]) + 0x10;
      }
    }
    else
    {
      var_a1 = Str_LookupTable8((*((u8 *) (((s8 *) temp_s0) + 4))) - 1);
      var_a0 = temp_s0;
    }
    block_14:
    Sfx_DrawSlotRow(var_a0, var_a1);

  }
}
