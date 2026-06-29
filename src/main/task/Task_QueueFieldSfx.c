/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    u16 field_0;
    u8 field_2;
    u8 field_3;
    int field_4;
    int field_8;
} struct_800A3180;

u8 g_FieldSfxQueueCount;
extern struct_800A3180 g_FieldSfxQueue[28];

void Task_QueueFieldSfx(int arg0, int arg1, int arg2, int arg3, int arg4) {
    struct_800A3180 *entry = &g_FieldSfxQueue[g_FieldSfxQueueCount];

    entry->field_3 = arg0;
    entry->field_2 = arg1;
    entry->field_8 = arg3;
    entry->field_0 = arg2;
    entry->field_4 = arg4;
    g_FieldSfxQueueCount++;
}
