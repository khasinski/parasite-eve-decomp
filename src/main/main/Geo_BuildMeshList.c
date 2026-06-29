/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK Render_DrawSpriteEntry();
extern struct { char _[16]; } g_GeomState_o __asm__("g_GeomState");
#define g_GeomState (*(void **)&g_GeomState_o)
extern struct { char _[16]; } D_800BCF8C_o __asm__("D_800BCF8C");
#define D_800BCF8C (*(u16 *)&D_800BCF8C_o)
extern struct { char _[16]; } D_800BCF8E_o __asm__("g_CameraClampedY");
#define g_CameraClampedY (*(u16 *)&D_800BCF8E_o)
extern struct { char _[16]; } g_GeomGroupSel_o __asm__("g_GeomGroupSel");
#define g_GeomGroupSel (*(u8 *)&g_GeomGroupSel_o)

s32 Geo_BuildMeshList(void) {
    register void *base asm("$4");
    void *entry;
    s32 i;
    register u16 count asm("$18");
    s32 temp;
    register s32 value asm("$3");
    s32 stack_pad[2];

    base = g_GeomState;
    temp = D_800BCF8C;
    temp -= 0xA0;
    value = *(volatile u16 *)(base + 0x2C);
    count = *(volatile u16 *)(base + 6);
    value -= temp;
    M2C_FIELD(base, s16 *, 0x38) = (s16) value;
    temp = g_CameraClampedY;
    value = M2C_FIELD(base, u16 *, 0x2E);
    temp -= 0x70;
    value -= temp;
    temp = M2C_FIELD(base, s32 *, 0x14);
    i = 0;
    M2C_FIELD(base, s16 *, 0x3A) = (s16) value;
    base += temp;
    if (count != 0) {
        entry = base;
        do {
            if ((M2C_FIELD(entry, u8 *, 0) & 2) && (M2C_FIELD(entry, u8 *, 0x24) == g_GeomGroupSel)) {
                Render_DrawSpriteEntry(entry);
            }
            i += 1;
            entry += 0x38;
        } while (i < (s32) count);
    }
    return 0;
}
