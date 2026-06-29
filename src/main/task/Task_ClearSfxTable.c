/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

typedef struct {
    short field_0;
    u8 field_2;
    u8 field_3;
    int field_4;
    int field_8;
} struct_800A3180;

u8 g_FieldSfxQueueCount;
extern struct_800A3180 g_FieldSfxQueue[28];

void Task_ClearSfxTable(void) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < 28; i++) {
        for (j = 0; j < 3; j++) {
            ((int *)&g_FieldSfxQueue[i])[j] = 0;
        }
    }
    g_FieldSfxQueueCount = 0;
}
