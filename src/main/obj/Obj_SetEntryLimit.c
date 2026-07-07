/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef struct GeomCtrlEntry
{
  union 
  {
    u32 packed;
    struct 
    {
      u8 flags;
      u8 _b[3];
    } b;
  } head;
  u16 field4;
  s16 field6;
  s16 field8;
  u16 fieldA;
  s32 slot_offset;
} GeomCtrlEntry;
typedef struct GeomEntry
{
  u8 flags;
  u8 pad01[3];
  u16 anim_mod_x;
  u16 anim_mod_y;
  u16 base_x;
  u16 base_y;
  u16 scr_x;
  u16 scr_y;
  s16 ot10;
  s16 ot12;
  s16 ot14;
  s16 ot16;
  u16 disp_x;
  u16 disp_y;
  s16 field1C;
  s16 field1E;
  s16 field20;
  s16 field22;
  u8 group;
  u8 pad25[1];
  u16 prim_count;
  union 
  {
    s32 pos_ptr;
    struct 
    {
      u16 size28;
      u16 size2A;
    } sz;
  } u28;
  union 
  {
    s32 uv_ptr;
    struct 
    {
      s16 min_x;
      s16 max_x;
    } bx;
  } u2C;
  union 
  {
    void *prim;
    struct 
    {
      s16 min_y;
      s16 max_y;
    } by;
  } u30;
  u8 pad34[4];
} GeomEntry;
typedef struct GeomState
{
  u8 pad00[4];
  u16 entry_count;
  u16 entry_count06;
  u8 pad08[8];
  s32 ctrl_offset;
  s32 entry_offset;
  u8 pad18[4];
  s32 entry_offset_1C;
  u8 pad20[6];
  u16 field26;
  u8 pad28[4];
  u16 disp_src_x;
  u16 disp_src_y;
  u8 pad30[8];
  s16 out_disp_x;
  s16 out_disp_y;
} GeomState;
extern GeomState * volatile g_GeomState;
int Obj_SetEntryLimit(int arg0, int arg1)
{
  GeomCtrlEntry *entry;
  u32 field4;
  register u32 flags;
  u32 hi;
  int ret;
  entry = (GeomCtrlEntry *) ((((u8 *) g_GeomState) + g_GeomState->ctrl_offset) + (arg0 << 4));
  field4 = *((u32 *) (&entry->field4));
  field4 &= 0xFF;
  ret = entry->head.b.flags;
  flags = ret;
  hi = arg1 << 16;
  entry->fieldA = 0;
  field4 |= hi;
  flags |= 2;
  entry->head.b.flags = flags;
  ret = 0;
  *((u32 *) (&entry->field4)) = field4;
  return ret;
}
