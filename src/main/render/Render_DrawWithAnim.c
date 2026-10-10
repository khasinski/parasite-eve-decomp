#include "common.h"
#include "pe1/entity_frame_update.h"
#include "pe1/render_matrix_copy.h"
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"


extern RenderMatrix g_IdentityMatrixSource;
extern struct { char _[16]; } D_8009CDDC_lh_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_lw0_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_sw0_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_lw1_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_sw1_o __asm__("g_ActiveDrawSlot");
#define D_8009CDDC_lh (*(s16 *)&D_8009CDDC_lh_o)
#define D_8009CDDC_lw0 (*(s32 *)&D_8009CDDC_lw0_o)
#define D_8009CDDC_sw0 (*(s32 *)&D_8009CDDC_sw0_o)
#define D_8009CDDC_lw1 (*(s32 *)&D_8009CDDC_lw1_o)
#define D_8009CDDC_sw1 (*(s32 *)&D_8009CDDC_sw1_o)
extern RenderMatrix g_RenderClutLookupTable;

void Render_DrawWithAnim(RenderObjectEntity *arg0, s32 arg1, s16 arg2, M2C_UNK arg3) {
    RenderMatrix *temp_s0;
    s32 temp_v0;

    if (arg1 != 0) {
        arg0->animation_data = (void *)arg1;
        temp_s0 = &arg0->model_matrix;
        Render_CopyMatrixBlock(temp_s0, &g_RenderClutLookupTable, 1);
        Render_CopyMatrixBlock(&g_IdentityMatrixSource, temp_s0, 1);
        Anim_BuildRotationMatrices(arg0, arg1, arg2, 0);
    }
    Render_TransformVertices(arg0);
    Render_CopyMatrixBlock(&g_RenderClutLookupTable, &arg0->model_matrix, 1);
    Render_DrawObject(arg0, arg3);
    Render_UpdateClutTable(arg0, 1, D_8009CDDC_lh);
    temp_v0 = D_8009CDDC_lw0 ^ 1;
    D_8009CDDC_sw0 = temp_v0;
    Render_UpdateClutTable(arg0, 1, (s16) temp_v0);
    D_8009CDDC_sw1 = D_8009CDDC_lw1 ^ 1;
}


/* Equivalent branches preserve the stock compiler\'s register allocation.
 * No packet operation depends on which of these identical branches is taken. */

void Render_InitPrimBlock(RenderObjectEntity *object, s16 x, s16 y, int unused, unsigned int palette_row)
{
    unsigned int page;
    int page_offset;
    unsigned int scaled;
    u16 signed_page;
    unsigned int palette_offset;
    unsigned int row_copy;
    s16 half_page;
    int flag_copy;
    RenderPacket34 *quad;
    RenderPacket28 *tri;
    s16 i;
    s16 side;
    half_page = y == 128;
    page = ((((x >> 6) - 8) * 2) + (y >> 7)) + ((y >> 8) * 30);
    signed_page = page;
    row_copy = palette_row;
    quad = (RenderPacket34 *) object->primitive_buffer;
    if (x != 960)
    {
        page_offset = page >> 1;
        scaled = palette_row * 64;
    }
    else
    {
        if (quad->page_bits == 31)
        {
            return;
        }
        if (quad)
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        else
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        if (quad)
        {
            scaled = row_copy * 64;
        }
        else
        {
            scaled = row_copy * 64;
        }
    }
    signed_page = page;
    if (signed_page)
    {
        palette_offset = scaled - 0x7080;
    }
    else
    {
        palette_offset = scaled - 0x7080;
    }
    for (i = 0; i < object->header->packet34_count; i++)
    {
        flag_copy = half_page;
        for (side = 0; side < 2; side++, quad++)
        {
            if (flag_copy)
            {
                u8 v = quad->v0;
                if (v < 128)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset;
                }
                else
                    if (object)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
                else
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
            }
            else
            {
                quad->page_bits += page_offset;
            }
            quad->clut += palette_offset;
        }

    }

    tri = (RenderPacket28 *) quad;
    for (i = 0; i < object->header->packet28_count; i++)
    {
        for (side = 0; side < 2; side++, tri++)
        {
            if (half_page)
            {
                u8 v = tri->v0;
                if (v < 128)
                {
                    tri->v0 = v + 128;
                    tri->v1 += 128;
                    tri->v2 += 128;
                    scaled = palette_row * 64;
                    tri->page_bits += page_offset;
                }
                else
                {
                    tri->v0 = v + 128;
                    tri->v1 = tri->v1 + 128;
                    tri->v2 += 128;
                    tri->page_bits += page_offset + 1;
                }
            }
            else
            {
                tri->page_bits += page_offset;
            }
            tri->clut += palette_offset;
        }

    }

}

#include "pe1/gte.h"

void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->model_matrix.rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->model_matrix.rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->model_matrix.rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->model_matrix.rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->model_matrix.rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->model_matrix.rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->model_matrix.rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->model_matrix.rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->model_matrix.rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->model_matrix.translation[0] = src->matrices[index].translation[0];
    dst->model_matrix.translation[1] = src->matrices[index].translation[1];
    dst->model_matrix.translation[2] = src->matrices[index].translation[2];
}

void Render_CopyFrameDataDouble(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->matrices[0].rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->matrices[0].rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->matrices[0].rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->matrices[0].rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->matrices[0].rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->matrices[0].rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->matrices[0].rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->matrices[0].rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->matrices[0].rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->matrices[0].translation[0] = src->matrices[index].translation[0];
    dst->matrices[0].translation[1] = src->matrices[index].translation[1];
    dst->matrices[0].translation[2] = src->matrices[index].translation[2];

    dst->matrices[1].rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->matrices[1].rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->matrices[1].rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->matrices[1].rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->matrices[1].rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->matrices[1].rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->matrices[1].rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->matrices[1].rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->matrices[1].rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->matrices[1].translation[0] = src->matrices[index].translation[0];
    dst->matrices[1].translation[1] = src->matrices[index].translation[1];
    dst->matrices[1].translation[2] = src->matrices[index].translation[2];
}

typedef unsigned short u16_1;

typedef unsigned char u8_2;
typedef short s16_2;

void Render_InitObjectFromTable(RenderObjectEntity *obj, RenderObjectEntity *owner, int index) {
    int offset;
    int base;
    RenderVec3s *entry;

    obj->table_index = index;
    offset = (short)index << 4;
    obj->header = 0;
    obj->animation_source = owner;

    base = (int)owner->bounds_vertices;
    entry = (RenderVec3s *)(offset + base);
    /* The first vector of each 16-byte bounds pair stores radius in pad. */
    obj->hit_cylinder.radius = (u16)entry->pad;

    base = (int)owner->bounds_vertices;
    entry = (RenderVec3s *)(offset + base);
    obj->table_value2c = (u16)entry->x;

    base = (int)owner->bounds_vertices;
    entry = (RenderVec3s *)(offset + base);
    obj->table_value2e = (u16)entry->y;

    base = (int)owner->bounds_vertices;
    offset += base;
    entry = (RenderVec3s *)offset;
    obj->table_value30 = (u16)entry->z + (u16)obj->hit_cylinder.radius;
}

void Render_Noop(void) {
}

int Render_ReturnZero(void) {
    return 0;
}

void Render_CopyMatrixBlock(RenderMatrix *arg0, RenderMatrix *arg1, s16 count) {
    char unused[8];
    register RenderMatrix *src_cur asm("$7");
    register RenderMatrix *dst_cur asm("$8");
    int i;
    register s32 *src_tail asm("$4");
    register s32 *dst_tail asm("$5");
    int tmp;

    src_cur = arg0;
    dst_cur = arg1;
    i = 0;
    if (count > 0) {
        dst_tail = &arg1->translation[2];
        src_tail = &arg0->translation[2];
        do {
            dst_cur->rotation[0][0] = src_cur->rotation[0][0];
            *(u16_1 *)((char *)dst_tail - 0x1A) = *(u16_1 *)((char *)src_tail - 0x1A);
            *(u16_1 *)((char *)dst_tail - 0x18) = *(u16_1 *)((char *)src_tail - 0x18);
            *(u16_1 *)((char *)dst_tail - 0x16) = *(u16_1 *)((char *)src_tail - 0x16);
            *(u16_1 *)((char *)dst_tail - 0x14) = *(u16_1 *)((char *)src_tail - 0x14);
            *(u16_1 *)((char *)dst_tail - 0x12) = *(u16_1 *)((char *)src_tail - 0x12);
            *(u16_1 *)((char *)dst_tail - 0x10) = *(u16_1 *)((char *)src_tail - 0x10);
            *(u16_1 *)((char *)dst_tail - 0xE) = *(u16_1 *)((char *)src_tail - 0xE);
            tmp = *(u16_1 *)((char *)src_tail - 0xC);
            i++;
            *(u16_1 *)((char *)dst_tail - 0xC) = tmp;
            tmp = src_tail[-2];
            src_cur++;
            dst_tail[-2] = tmp;
            tmp = src_tail[-1];
            dst_cur++;
            dst_tail[-1] = tmp;
            tmp = *src_tail;
            src_tail += 8;
            *dst_tail = tmp;
            dst_tail += 8;
        } while (i < count);
    }
}

void Render_SetObjectAnim(RenderObjectEntity *arg0, RenderObjectEntity *arg1, short arg2) {
    if (arg0->header->part_count == 2) {
        arg0->animation_source = arg1;
        arg0->animation_state = 3;
    } else {
        arg0->animation_source = arg1;
        arg0->animation_state = 1;
    }
    arg0->animation_id = arg2;
}

void Render_ClearObjectAnim(RenderObjectEntity *arg0) {
    arg0->animation_source = 0;
    if (arg0->header->part_count == 2) {
        arg0->animation_state = 2;
    } else {
        arg0->animation_state = 0;
    }
}

int Render_FindAnimEntry(RenderObjectEntity *arg0, int arg1, s32 *out) {
    char unused[8];
    RenderAnimationLookupEntry *base;
    register s16 *entry asm("$3");
    s32 i;
    s32 tmp;

    tmp = arg0->header->animation_entry_count;
    base = arg0->animation_entries;
    i = 0;
    if (tmp > 0) {
        tmp = arg1 << 16;
        arg1 = tmp >> 16;
        entry = &base->value2;
        do {
            tmp = entry[1];
            i++;
            if (tmp == arg1) {
                out[0] = base->value0;
                out[1] = entry[-1];
                out[2] = *entry;
                return 1;
            }
            entry += 6;
            tmp = arg0->header->animation_entry_count;
            base++;
        } while (i < tmp);
    }

    return 0;
}

/* Matching debt: transfer and scratch pointers are pinned, with empty
 * address/memory constraints. Physical scratchpad addresses and pointer
 * casts remain. CPU loads, stores and vector packing are C; GTE transfers
 * and commands use individual macros. */
#define BoneLoadRotMatrix(matrix) gte_ldrotmatrix((const GteMatrixWords *)(matrix))

#define BoneLoadTrans(matrix) gte_ldtransmatrix((const GteMatrixWords *)(matrix))

#define BoneLoadFullMatrix(matrix)                                                                 \
    {                                                                                              \
        BoneLoadRotMatrix(matrix);                                                                 \
        BoneLoadTrans(matrix);                                                                     \
    }

#define Bone_LoadAxis(src) \
    { \
        gte_ldclmv((src)); \
        gte_rtir(); \
    }

#define Bone_StoreAxis(dst) gte_stclmv((dst))

#define Bone_StoreVec(output_expr)                                                                 \
    {                                                                                              \
        s16 *out = (s16 *)(output_expr);                                                           \
        gte_stsv(out); \
    }
void Render_SetupBoneTransforms(RenderObjectEntity *input, s32 *view_input) {
    s32 *scratch = (s32 *)0x1F800000;
    RenderObjectEntity *actor = input;
    s32 *view = view_input;
    RenderObjectEntity *source;
    int index;
    s32 *matrix;
        source = actor->animation_source;
    index = (s16)actor->table_index;
    matrix = (s32 *)source->matrices;
    {
        int offset = index * 32;

        matrix = (s32 *)((u8 *)matrix + offset);
    }
    BoneLoadFullMatrix(matrix);
    {
        u8 *point = (u8 *)source->bounds_vertices;
        index *= 16;
        point += index;

        gte_lwc2_0_0(point);
        gte_lwc2_1_4(point);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
    }
    /* Headerless objects reuse the first override XYZ as a position output. */
    Bone_StoreVec(&actor->rotation_overrides[0].x);
    Bone_StoreVec(&actor->hit_cylinder.value0);
    Bone_StoreVec(&actor->target_x);
    {
        register s16 *column asm("$9");
        BoneLoadRotMatrix(view);
        Bone_LoadAxis((u16 *)matrix);
        Bone_StoreAxis((s16 *)scratch);
        {
            u16 *src = (u16 *)matrix + 1;
            Bone_LoadAxis(src);
            column = (s16 *)0x1F800002;
            Bone_StoreAxis(column);
        }
        {
            u16 *src = (u16 *)matrix + 2;
            Bone_LoadAxis(src);
            column = (s16 *)0x1F800004;
            Bone_StoreAxis(column);
        }
    }
    BoneLoadTrans(view);
    matrix += 5;

    {
        gte_ldlv0(matrix);
        gte_rt();
        {
            register s32 *dst asm("$9") = (s32 *)0x1F800014;

            gte_swc2_25_0(dst);
            gte_swc2_26_4(dst);
            gte_swc2_27_8(dst);
        }
    }
    BoneLoadFullMatrix(scratch);
    /* In this mode the halfwords at +0x2C form the projection input vector. */
    {
        u16 *point = &actor->table_value2c;

        gte_lwc2_0_0(point);
        gte_lwc2_1_4(point);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        actor->hit_body.radius = source->hit_body.radius;
        {
            s16 *out = &actor->projected_target_x;
            gte_stsxy2(out);
        }
        actor->table_value2c += (u16)actor->hit_cylinder.radius;
        gte_lwc2_0_0(point);
        gte_lwc2_1_4(point);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        actor->hit_body.value0 = source->hit_body.value0;
        actor->hit_body.value1 = source->hit_body.value1;
        actor->hit_body.value2 = source->hit_body.value2;
        {
            s16 *out = &actor->projected_x;
            gte_stsxy2(out);
        }
        actor->table_value2c -= (u16)actor->hit_cylinder.radius;
    }
}

void Render_OffsetObjectTextureCoordinates(RenderObjectEntity *object, int du, int dv, int clutOffset) {
    u8 savedClut = clutOffset;
    RenderPacket34 *quad = (RenderPacket34 *)object->primitive_buffer;
    RenderPacket28 *tri;
    int i, j;
    u8 triClut;
    for (i = 0; i < object->header->packet34_count; i++) {
        for (j = 0; j < 2; j++, quad++) {
            quad->v0 += dv;
            quad->u0 += du;
            quad->v1 += dv;
            quad->u1 += du;
            quad->v2 += dv;
            quad->u2 += du;
            /* Retail leaves u3 unchanged. */
            quad->v3 += dv;
            quad->clut += (s8)clutOffset;
        }
    }
    triClut = savedClut;
    /* Byte-sized copies preserve the two signed CLUT conversions in plain C. */
    tri = (RenderPacket28 *)quad;
    for (i = 0; i < object->header->packet28_count; i++) {
        for (j = 0; j < 2; j++, tri++) {
            tri->v0 += dv;
            tri->u0 += du;
            tri->v1 += dv;
            tri->u1 += du;
            tri->v2 += dv;
            tri->u2 += du;
            tri->clut += (s8)triClut;
        }
    }
}

extern int D_8003E60C;

void Render_IncrementCounter(void) {
    register int *ptr asm("$4");
    register int value asm("$5");

    ptr = &D_8003E60C;
    value = *ptr;
    value++;
    *ptr = value;
}
