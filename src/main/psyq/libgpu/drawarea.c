#include "pe1/gpu_command_builders.h"
#include "common.h"
/* CC1_FLAGS: -O1 */

extern s16 D_80095750;
extern s16 D_80095752;

int Gpu_BuildDrawAreaTopLeftCmd(int x, int y) {
    register int clamped_x asm("$2");
    int signed_coord;
    int packed_y;
    int value;
    int shifted;
    int limit;
    int exceeds_limit;

    shifted = x << 16;
    signed_coord = shifted >> 16;
    clamped_x = 0;
    if (signed_coord >= 0) {
        exceeds_limit = D_80095750 - 1 < signed_coord;
        limit = (u16)D_80095750;
        if (exceeds_limit) {
            value = limit;
            clamped_x = value - 1;
        } else {
            clamped_x = x;
        }
    }

    x = clamped_x;
    shifted = y << 16;
    signed_coord = shifted >> 16;
    if (signed_coord >= 0) {
        exceeds_limit = D_80095752 - 1 < signed_coord;
        limit = (u16)D_80095752;
        y = exceeds_limit ? limit - 1 : y;
    } else {
        y = 0;
    }
    packed_y = y & 0x3FF;

    packed_y <<= 10;
    clamped_x = x & 0x3FF;
    value = 0xE3000000;
    clamped_x |= value;
    clamped_x = packed_y | clamped_x;
    return clamped_x;
}

int Gpu_BuildDrawAreaBottomRightCmd(int x, int y) {
    register int clamped_x asm("$2");
    int signed_coord;
    int packed_y;
    int value;
    int shifted;
    int limit;
    int exceeds_limit;

    shifted = x << 16;
    signed_coord = shifted >> 16;
    clamped_x = 0;
    if (signed_coord >= 0) {
        exceeds_limit = D_80095750 - 1 < signed_coord;
        limit = (u16)D_80095750;
        if (exceeds_limit) {
            value = limit;
            clamped_x = value - 1;
        } else {
            clamped_x = x;
        }
    }

    x = clamped_x;
    shifted = y << 16;
    signed_coord = shifted >> 16;
    if (signed_coord >= 0) {
        exceeds_limit = D_80095752 - 1 < signed_coord;
        limit = (u16)D_80095752;
        y = exceeds_limit ? limit - 1 : y;
    } else {
        y = 0;
    }
    packed_y = y & 0x3FF;

    packed_y <<= 10;
    clamped_x = x & 0x3FF;
    value = 0xE4000000;
    clamped_x |= value;
    clamped_x = packed_y | clamped_x;
    return clamped_x;
}
