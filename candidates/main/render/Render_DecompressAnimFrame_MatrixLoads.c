/* Matching debt: pinned GTE transfer words and translation address, empty
 * address/memory constraints, and the existing artificial spill reservation.
 * Full C candidate: score 30; the initial matrix address differs from retail. */
#include "common.h"
#include "m2c_macros.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/render_anim_position.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct RenderAnimFrame {
    u8 unk0;
    u8 vertex_count;
    u8 pad02[4];
    u16 data_offset_words;
    u8 count4_a;
    u8 count4_b;
    u8 face_count;
    u8 count4_c;
    u8 count8_a;
    u8 count8_b;
    u8 count8_c;
    u8 count8_d;
    u8 count4_d;
    u8 count2_a;
    u8 count2_b;
    u8 modifier_count;
    u8 pad14[8];
} RenderAnimFrame;

typedef union RenderAnimScratchWord {
    u32 raw;
    u8 bytes[4];
} RenderAnimScratchWord;

typedef struct RenderAnimVertex {
    s16 x;
    s16 y;
    s16 z;
    u8 material_index;
    u8 unk7;
} RenderAnimVertex;

typedef struct RenderAnimMaterialRecord {
    u8 reserved[3];
    u8 flags;
} RenderAnimMaterialRecord;

typedef union RenderAnimTag {
    u32 raw;
    struct {
        u32 addr : 24;
        u32 len : 8;
    } bits;
} RenderAnimTag;

typedef struct RenderAnimLineG2 {
    RenderAnimTag tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    u32 xy0;
    u8 r1;
    u8 g1;
    u8 b1;
    u8 pad0F;
    u32 xy1;
} RenderAnimLineG2;

typedef struct RenderAnimFace {
    u8 vertex0;
    u8 vertex1;
    u8 vertex2;
    u8 unk3;
} RenderAnimFace;

PE1_STATIC_ASSERT(sizeof(RenderAnimFrame) == 0x1C, render_anim_frame_size);
PE1_STATIC_ASSERT(sizeof(RenderAnimVertex) == 8, render_anim_vertex_size);
PE1_STATIC_ASSERT(sizeof(RenderAnimMaterialRecord) == 4,
                  render_anim_material_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimMaterialRecord, flags) == 3,
                  render_anim_material_flags_offset);
PE1_STATIC_ASSERT(sizeof(RenderAnimLineG2) == 0x14, render_anim_line_g2_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimLineG2, xy0) == 8,
                  render_anim_line_g2_xy0_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderAnimLineG2, xy1) == 0x10,
                  render_anim_line_g2_xy1_offset);
PE1_STATIC_ASSERT(sizeof(RenderAnimFace) == 4, render_anim_face_size);

struct _m2c_stack_Render_DecompressAnimFrame {
    /* 0x00 */ RenderMatrix matrix;
    /* 0x20 */ s32 sp30;
};

GteMatrix *RotMatrixY(s32, GteMatrix *);
void SetDrawMode(void *, s32, s32, s32);
extern u32 *D_8009CDD0;
extern s16 *D_8009CDD4;

#define AnimLoadRotMatrix(matrix) \
    { \
        register u32 x asm("$12"), y asm("$13"), z asm("$14"); \
        x = ((const u32 *)(matrix))[0]; \
        y = ((const u32 *)(matrix))[1]; \
        gte_ctc2_0(x); \
        gte_ctc2_1(y); \
        x = ((const u32 *)(matrix))[2]; \
        y = ((const u32 *)(matrix))[3]; \
        z = ((const u32 *)(matrix))[4]; \
        gte_ctc2_2(x); \
        gte_ctc2_3(y); \
        gte_ctc2_4(z); \
    }

#define AnimLoadTrans(matrix) \
    { \
        register u32 x asm("$12"), y asm("$13"), z asm("$14"); \
        x = ((const u32 *)(matrix))[5]; \
        y = ((const u32 *)(matrix))[6]; \
        gte_ctc2_5(x); \
        z = ((const u32 *)(matrix))[7]; \
        gte_ctc2_6(y); \
        gte_ctc2_7(z); \
    }


#define Anim_LoadAxis(src) \
    { \
        register u32 x asm("$12"), y asm("$13"), z asm("$14"); \
        x = ((const u16 *)(src))[0]; \
        y = ((const u16 *)(src))[3]; \
        z = ((const u16 *)(src))[6]; \
        gte_mtc2_9(x); \
        gte_mtc2_10(y); \
        gte_mtc2_11(z); \
        gte_cop2_hazard_slot(); \
        gte_cop2_hazard_slot(); \
        gte_mvmva_light_ir_sf0(); \
    }

#define Anim_StoreAxis(dst) \
    { \
        register u32 x asm("$12"), y asm("$13"), z asm("$14"); \
        gte_mfc2_9(x); \
        gte_mfc2_10(y); \
        gte_mfc2_11(z); \
        (dst)[0] = x; \
        (dst)[3] = y; \
        (dst)[6] = z; \
        asm volatile("" : : : "memory"); \
    }


#define AnimLoadVector(source) \
    do { \
        register unsigned int packed asm("$12"); \
        register unsigned int high asm("$13"); \
        asm volatile("" : "=r"(source) : "0"(source)); \
        high = ((volatile unsigned short *)source)[2]; \
        packed = ((unsigned short *)source)[0]; \
        high <<= 16; \
        packed |= high; \
        gte_mtc2_0(packed); \
        gte_lwc2_1_8(source); \
    } while (0)

s32 Render_DecompressAnimFrame(RenderAnimFrame *frame, void *arg1, s32 arg2, s32 arg3, u32 *arg4, u8 *arg5) {
    struct _m2c_stack_Render_DecompressAnimFrame stack;
    s32 temp_v0_2;
    s16 temp_v0_2_saved;
    u32 *temp_a0_2;
    u32 *temp_a0_3;
    u32 *temp_a0_4;
    u32 *temp_a0_5;
    register s16 *depth_out asm("$2");
    RenderAnimLineG2 *negative_line;
    RenderAnimLineG2 *positive_line;
    s16 temp_s1;
    u32 temp_fp;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s32 temp_v1_7;
    u32 var_a2;
    s32 positive_depth;
    s32 var_s4;
    s32 var_t0;
    u32 var_t3;
    u32 var_t4;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_t6;
    s32 var_v0_3;
    u32 var_t9;
    u32 address_mask;
    u32 command_mask;
    u16 temp_a0;
    u16 temp_v1_2;
    u8 *var_a0;
    u32 temp_v0;
    u32 temp_v0_3;
    u8 temp_a3;
    u8 temp_a3_2;
    u8 special_code;
    s32 temp_s5;
    s32 temp_s6;
    s32 temp_s7;
    s32 temp_v1;
    u8 *material_base;
    s32 item_count;
    s16 var_t1;
    s32 primitive_code;
    s16 var_s0;
    u8 var_v1;
    u8 selected_vertex;
    u8 *temp_a2;
    u8 *var_s3;
    u8 *var_s2;
    u8 *output_start;
    /* Match note: these constructor pseudos reproduce GCC's unused sp50-sp6F spill range. */
    register RenderAnimScratchWord spill0 = {0}, spill1 = {0}, spill2 = {0}, spill3 = {0};
    RenderAnimVertex *var_t7;
    RenderAnimFace *face_cursor;
    RenderAnimVertex *vertex_record;
    s32 **matrix_slot;
    s32 *matrix_value;
    RenderAnimPositionInput *position_input;

    var_s2 = arg5;
    var_a2 = arg2;
    var_v0 = arg2 >> 0x10;
    output_start = var_s2;
    if (var_v0 < 0) {
        var_v0 += 3;
    }
    temp_v0 = (u32)var_v0 >> 2;
    stack.matrix.rotation[2][2] = (s16) temp_v0;
    stack.matrix.rotation[1][1] = (s16) temp_v0;
    stack.matrix.rotation[0][0] = (s16) temp_v0;
    stack.matrix.rotation[2][1] = 0;
    stack.matrix.rotation[2][0] = 0;
    stack.matrix.rotation[1][2] = 0;
    stack.matrix.rotation[1][0] = 0;
    stack.matrix.rotation[0][2] = 0;
    stack.matrix.rotation[0][1] = 0;
    position_input = arg1;
    stack.matrix.translation[0] = position_input->x;
    stack.matrix.translation[1] = position_input->y;
    stack.matrix.translation[2] = position_input->z;
    RotMatrixY(arg3 & 0xFFFF, (GteMatrix *)&stack.matrix);
    matrix_slot = &D_800BCFA4.value;
    matrix_value = *matrix_slot;
    AnimLoadRotMatrix(matrix_value);
    {
        short *column = &stack.matrix.rotation[0][0];
        asm volatile("" : "=r"(column) : "0"(column));
        Anim_LoadAxis(column);
        Anim_StoreAxis(column);
        {
            u16 *src = (u16 *)&stack.matrix.rotation[0][1];
            asm volatile("" : "=r"(src) : "0"(src));
            {
                volatile s16 *out = (volatile s16 *)src;
                Anim_LoadAxis(src);
                Anim_StoreAxis(out);
            }
        }
        {
            u16 *src = (u16 *)&stack.matrix.rotation[0][2];
            asm volatile("" : "=r"(src) : "0"(src));
            {
                volatile s16 *out = (volatile s16 *)src;
                Anim_LoadAxis(src);
                Anim_StoreAxis(out);
            }
        }
        {
            register s32 *transMatrix asm("$4") = *matrix_slot;
            AnimLoadTrans(transMatrix);
        }
        {
            unsigned short *translation = (unsigned short *)&stack.matrix.translation[0];
            AnimLoadVector(translation);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtv0tr_sf0();
            gte_swc2_9_0(translation);
            gte_swc2_10_4(translation);
            gte_swc2_11_8(translation);
        }
        AnimLoadRotMatrix(column);
        AnimLoadTrans(column);
    }
    var_t6 = 0;
    item_count = frame->vertex_count;
    var_t7 = (RenderAnimVertex *)(frame + 1);
    if (item_count != 0) {
        var_a0 = (u8 *)var_t7;
        do {
            gte_ldv0(var_a0);
            gte_rtps();
            gte_stsxy2(&D_8009CDD0[var_t6]);
            depth_out = &D_8009CDD4[var_t6];
            {
                register int depth asm("$12");
                gte_getsz3_for_store(depth, depth_out);
                *depth_out = depth;
            }
            var_a0 += 8;
            var_t6 += 1;
        } while (var_t6 < item_count);
        var_t6 = 0;
    }
    temp_a2 = (u8 *)&var_t7[frame->vertex_count] + ((frame->count4_a + frame->count4_b) * 4);
    var_a0 = temp_a2 + ((frame->face_count + frame->count4_c) * 4) + ((frame->count8_a + frame->count8_b + frame->count8_c + frame->count8_d) * 8) + (frame->count4_d * 4) + (frame->count2_a * 2) + (frame->count2_b * 2);
    temp_v1 = frame->modifier_count;
    material_base = (u8 *)frame + (frame->data_offset_words * 4);
    if (temp_v1 != 0) {
        do {
            var_t6 += 1;
            *var_a0 &= 0x7F;
            var_a0 += 2;
        } while (var_t6 < temp_v1);
    }
    var_t6 = 0;
    item_count = frame->face_count;
    if (item_count != 0) {
        var_t9 = 0xFF;
        address_mask = 0xFFFFFF;
        command_mask = 0xFF000000;
        face_cursor = (RenderAnimFace *)temp_a2;
        do {
            temp_s6 = face_cursor->vertex0;
            temp_s7 = face_cursor->vertex1;
            temp_s5 = face_cursor->vertex2;
            var_t4 = D_8009CDD0[temp_s6];
            temp_fp = D_8009CDD0[temp_s7];
            var_t3 = D_8009CDD0[temp_s5];
            gte_ldsxy0(var_t4);
            gte_ldsxy2(var_t3);
            gte_ldsxy1(temp_fp);
            gte_nclip();
            gte_stmac0(&stack.sp30);
            var_s4 = 0;
            primitive_code = 0x50;
            if (stack.sp30 <= 0) {
                s32 negative_depth;
                u8 negative_material_flags;

                var_t1 = -1;
                var_s0 = -1;
                var_a2 = -1;
                var_t0 = -1;
                temp_v0_2 = var_t7[temp_s6].y;
                temp_a0 = (u16)var_t7[temp_s7].y;
                temp_v1_2 = (u16)var_t7[temp_s5].y;
                temp_v0_2_saved = temp_v0_2;
                if (temp_v0_2 == 0) {
                    if (temp_a0 != 0) {
                        goto block_17;
                    }
                    var_s0 = temp_s6;
                    var_t1 = temp_s7;
                    var_t0 = var_t4;
                    var_a2 = temp_fp;
                    goto block_21;
                }
                if (temp_a0 != 0) {
                    goto block_17;
                }
                if (temp_v1_2 != 0) {
                    goto block_17;
                }
                var_t1 = temp_s5;
                var_t0 = temp_fp;
                goto block_20;
block_17:
                var_v0_2 = temp_v0_2_saved << 0x10;
                if (var_v0_2 != 0) {
                    goto block_21;
                }
                var_v0_3 = var_s0 << 0x10;
                if (temp_v1_2 != 0) {
                    goto block_22;
                }
                var_s0 = temp_s6;
                var_t1 = temp_s5;
                var_t0 = var_t4;
block_20:
                var_a2 = var_t3;
block_21:
                var_v0_3 = var_s0 << 0x10;
block_22:
                temp_v1_3 = var_v0_3 >> 0x10;
                if ((temp_v1_3 != -1) && ((s16)var_t1 != -1)) {
                    negative_line = (RenderAnimLineG2 *)var_s2;
                    temp_a3 = ((RenderAnimMaterialRecord *)(material_base +
                                  var_t7[temp_v1_3].material_index * 4))->flags;
                    negative_material_flags = temp_a3;
                    negative_line->tag.bits.len = 4;
                    negative_line->code = primitive_code;
                    if (negative_material_flags & 0x20) {
                        if (var_t4 || var_t3) {
                            special_code = 0x52;
                        } else {
                            special_code = 0x52;
                        }
                        M2C_FIELD(var_s2, u8 *, 7) = special_code;
                    }
                    negative_line->r0 = 0;
                    negative_line->g0 = var_t9;
                    negative_line->b0 = var_t9;
                    negative_line->r1 = 0;
                    negative_line->g1 = var_t9;
                    negative_line->b1 = var_t9;
                    *((volatile u32 *)&negative_line->xy0) = var_t0;
                    *((volatile u32 *)&negative_line->xy1) = var_a2;
                    temp_v1_5 = (u16)var_t1 << 16;
                    temp_v1_4 = D_8009CDD4[var_s0] +
                                D_8009CDD4[(s16)((u16)var_t1 | temp_v1_5)];
                    negative_depth = temp_v1_4 >> 3;
                    if (temp_v1_4 < 0) {
                        temp_v1_4 += 7;
                        negative_depth = temp_v1_4 >> 3;
                    }
                    temp_s1 = negative_depth < 0x1000;
                    if (temp_s1 != 0) {
                        var_s2 += 0x14;
                temp_a0_2 = arg4 - (-negative_depth);
                        negative_line->tag.raw = (negative_line->tag.raw & command_mask) | (((RenderAnimLineG2 *)temp_a0_2)->tag.raw & address_mask);
                        ((RenderAnimLineG2 *)temp_a0_2)->tag.raw = (((RenderAnimLineG2 *)temp_a0_2)->tag.raw & command_mask) | ((u32)negative_line & address_mask);
                    }
                    if (temp_a3 & 0x20) {
                        SetDrawMode(var_s2, 0, 1, (temp_a3 & 3) << 5);
                        if (temp_s1 != 0) {
                temp_a0_3 = arg4 - (-negative_depth);
                            ((RenderAnimLineG2 *)var_s2)->tag.raw = (((RenderAnimLineG2 *)var_s2)->tag.raw & command_mask) | (((RenderAnimLineG2 *)temp_a0_3)->tag.raw & address_mask);
                            ((RenderAnimLineG2 *)temp_a0_3)->tag.raw = (((RenderAnimLineG2 *)temp_a0_3)->tag.raw & command_mask) | ((u32)var_s2 & address_mask);
                            var_s2 += 8;
                        }
                    }
                } else {
                    M2C_FIELD(var_s2, s32 *, 0) = 0;
                    var_s2 += 4;
                }
            } else {
                var_s3 = (u8 *)var_s2 + 7;
                do {
                    selected_vertex = 0;
                    var_t0 = 0;
                    var_a2 = 0;
                    var_a0 = (u8 *)0;
                    switch (var_s4) {
                        case 0:
                            selected_vertex = temp_s6;
                            if (temp_s6 < temp_s7) {
                                var_t0 = temp_s7;
                                var_a2 = var_t4;
                                var_a0 = (u8 *)temp_fp;
                            } else {
                                selected_vertex = temp_s7;
                                var_t0 = temp_s6;
                                var_a2 = temp_fp;
                                var_a0 = (u8 *)var_t4;
                            }
                            break;
                        case 1:
                            selected_vertex = temp_s6;
                            if (temp_s6 < temp_s5) {
                                var_t0 = temp_s5;
                                var_a2 = var_t4;
                                var_a0 = (u8 *)var_t3;
                            } else {
                                selected_vertex = temp_s5;
                                var_t0 = temp_s6;
                                var_a2 = var_t3;
                                var_a0 = (u8 *)var_t4;
                            }
                            break;
                        case 2:
                            selected_vertex = temp_s7;
                            if (temp_s7 < temp_s5) {
                                var_t0 = temp_s5;
                                var_a2 = temp_fp;
                                var_a0 = (u8 *)var_t3;
                            } else {
                                selected_vertex = temp_s5;
                                var_t0 = temp_s7;
                                var_a2 = var_t3;
                                var_a0 = (u8 *)temp_fp;
                            }
                            break;
                    }
                    var_v1 = selected_vertex;
                    positive_line = (RenderAnimLineG2 *)var_s2;
                    vertex_record = (RenderAnimVertex *)((var_v1 * 8) + (u32)var_t7);
                    temp_a3_2 = ((RenderAnimMaterialRecord *)(material_base +
                                    vertex_record->material_index * 4))->flags;
                    M2C_FIELD(var_s3, s8 *, -4) = 4;
                    M2C_FIELD(var_s3, s8 *, 0) = primitive_code;
                    if (temp_a3_2 & 0x20) {
                        if (var_t3) {
                            M2C_FIELD(var_s3, u8 *, 0) = 0x52;
                        } else {
                            M2C_FIELD(var_s3, u8 *, 0) = 0x52;
                        }
                    }
                    if ((var_t7[var_v1].y == 0) && (var_t7[var_t0].y == 0)) {
                        positive_line->r0 = 0;
                        positive_line->g0 = var_t9;
                        positive_line->b0 = var_t9;
                        positive_line->r1 = 0;
                        positive_line->g1 = var_t9;
                        positive_line->b1 = var_t9;
                    } else {
                        positive_line->r0 = 0;
                        positive_line->g0 = 0x80;
                        positive_line->b0 = 0;
                        positive_line->r1 = 0;
                        positive_line->g1 = 0x80;
                        positive_line->b1 = 0;
                    }
                    *((volatile u32 *)&positive_line->xy0) = var_a2;
                    positive_line->xy1 = (u32)var_a0;
                    temp_v1_6 = D_8009CDD4[var_v1] + D_8009CDD4[var_t0];
                    positive_depth = temp_v1_6 >> 3;
                    if (temp_v1_6 < 0) {
                        temp_v1_6 += 7;
                        positive_depth = temp_v1_6 >> 3;
                    }
                    temp_s1 = positive_depth < 0x1000;
                    if (temp_s1 != 0) {
                        var_s3 += 0x14;
                        var_s2 += 0x14;
                temp_a0_4 = arg4 - (-positive_depth);
                        temp_v0_3 = ((RenderAnimLineG2 *)temp_a0_4)->tag.raw & address_mask;
                        temp_v1_7 = positive_line->tag.raw & command_mask;
                        temp_v1_7 |= temp_v0_3;
                        positive_line->tag.raw = temp_v1_7;
                        ((RenderAnimLineG2 *)temp_a0_4)->tag.raw = (((RenderAnimLineG2 *)temp_a0_4)->tag.raw & command_mask) | ((u32)positive_line & address_mask);
                    }
                    if (temp_a3_2 & 0x20) {
                        SetDrawMode(var_s2, 0, 1, (temp_a3_2 & 3) << 5);
                        if (temp_s1 != 0) {
                            do {
                                var_s3 += 8;
                temp_a0_5 = arg4 - (-positive_depth);
                                ((RenderAnimLineG2 *)var_s2)->tag.raw = (((RenderAnimLineG2 *)var_s2)->tag.raw & command_mask) | (((RenderAnimLineG2 *)temp_a0_5)->tag.raw & address_mask);
                            } while (0);
                            do {
                                temp_v1_7 = (u32)var_s2 & address_mask;
                                temp_v0_3 = ((RenderAnimLineG2 *)temp_a0_5)->tag.raw;
                                var_s2 += 8;
                                temp_v0_3 &= command_mask;
                                temp_v0_3 |= temp_v1_7;
                                ((RenderAnimLineG2 *)temp_a0_5)->tag.raw = temp_v0_3;
                            } while (0);
                        }
                    }
                    var_s4 += 1;
                } while (var_s4 < 3);
            }
            face_cursor += 1;
            var_t6 += 1;
        } while (var_t6 < item_count);
    }
    return var_s2 - output_start;
}
