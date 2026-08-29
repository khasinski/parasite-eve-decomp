#include "common.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 *D_8009D1FC;
extern u8 *volatile D_8009D1D8;
extern u8 **D_8009CE08;

int Geo_PointInTri(void *triangle, int x, int z);
int Math_FixedMul(int a, int b);

void Entity_FindFloor(u8 *entity)
{
    u8 *floor_data;
    u8 *simple_triangle;
    u8 *complex_triangle;
    u16 *initial_indices;
    u16 *indices;
    u16 triangle_count;
    u32 floor_index;
    u32 triangle_index;
    u16 triangle_id;
    int floor_value;
    s16 x;
    s16 z;
    int x_height;
    int z_height;
    int floor_offset;
    u32 floor_table_offset;
    u8 scratch[8];

    if ((U32_AT(entity, 0x98) & 0x80) != 0) {
        return;
    }

    x = U16_AT(entity, 0x2A);
    z = U16_AT(entity, 0x32);
    if (U16_AT(D_8009D1FC, 2) == 0) {
        return;
    }

    floor_index = 0;
    do {
        if (D_8009D1D8 == 0) {
            floor_table_offset = floor_index << 2;
            floor_table_offset += (u32)D_8009CE08;
            floor_data = *(u8 *volatile *)floor_table_offset;
            triangle_index = 0;
            floor_value = S16_AT(floor_data, 0);
            triangle_count = U16_AT(floor_data, 2);
            initial_indices = (u16 *)(floor_data + 4);
            indices = initial_indices;
            for (triangle_index = 0; triangle_index < triangle_count;
                 triangle_index++, indices++) {
                    triangle_id = *indices;
                    floor_offset = 11 * triangle_id;
                    simple_triangle = PTR_AT(D_8009D1FC, 0x1C) +
                                      floor_offset * 2;
                    if (Geo_PointInTri(simple_triangle, (s16)x, (s16)z)) {
                        PTR_AT(entity, 0x1A4) = simple_triangle;
                        PTR_AT(entity, 0x1A8) = simple_triangle;
                        if ((U32_AT(entity, 0x98) & 2) == 0) {
                            S32_AT(entity, 0x2C) = floor_value << 16;
                        }
                        return;
                    }
            }
        } else {
            floor_table_offset = floor_index << 2;
            floor_table_offset += (u32)D_8009CE08;
            floor_data = *(u8 *volatile *)floor_table_offset;
            triangle_index = 0;
            triangle_count = U16_AT(floor_data, 4);
            initial_indices = (u16 *)(floor_data + 6);
            indices = initial_indices;
            for (triangle_index = 0; triangle_index < triangle_count;
                 triangle_index++, indices++) {
                    triangle_id = *indices;
                    floor_offset = triangle_id * 28;
                    complex_triangle = PTR_AT(D_8009D1FC, 0x1C) + floor_offset;
                    if (Geo_PointInTri(complex_triangle, (s16)x, (s16)z)) {
                        PTR_AT(entity, 0x1A4) = complex_triangle;
                        PTR_AT(entity, 0x1A8) = complex_triangle;
                        if ((U32_AT(entity, 0x98) & 2) == 0) {
                            x_height = Math_FixedMul(
                                S32_AT(D_8009D1D8 +
                                           U16_AT(complex_triangle, 2) * 12,
                                       0),
                                S32_AT(entity, 0x28));
                            z_height = Math_FixedMul(
                                S32_AT(D_8009D1D8 +
                                           U16_AT(complex_triangle, 2) * 12,
                                       8),
                                S32_AT(entity, 0x30));
                            S32_AT(entity, 0x2C) = Math_FixedMul(
                                S32_AT(complex_triangle, 4) - x_height - z_height,
                                S32_AT(D_8009D1D8 +
                                           U16_AT(complex_triangle, 2) * 12,
                                       4));
                        }
                        return;
                    }
            }
        }
        floor_index++;
    } while (floor_index < U16_AT(D_8009D1FC, 2));
}
