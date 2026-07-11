typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern s32 g_ActiveDrawSlot __asm__("D_8009CDDC");

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

void RotMatrix(void *r, void *m);
void Anim_SetInterpRate(void *obj, int rate);
void Render_InitPrimBlock(void *obj, int arg3, int arg4, int arg5, int arg6);

static u8 *Render_InitPrimCodes(u8 *prim, u8 *src, int count, int stride, int code, int gpu_code,
                                int special_type, int special_gpu_code) {
    int i;

    for (i = 0; i < count; i++) {
        int type;

        type = src[3];

        prim[3] = code;
        prim[7] = gpu_code;
        if (type == special_type) {
            prim[7] = special_gpu_code;
        }
        prim += stride;

        prim[3] = code;
        prim[7] = gpu_code;
        if (type == special_type) {
            prim[7] = special_gpu_code;
        }
        prim += stride;

        src += 0xC;
    }

    return prim;
}

static u8 *Render_CopyQuadPrimData(u8 *dst, u8 *src, int count) {
    int i;
    int side;

    for (i = 0; i < count; i++) {
        for (side = 0; side < 2; side++) {
            dst[0x0C] = src[0x00];
            dst[0x0D] = src[0x01];
            U16_AT(dst, 0x0E) = U16_AT(src, 0x02);
            dst[0x18] = src[0x04];
            dst[0x19] = src[0x05];
            U16_AT(dst, 0x1A) = U16_AT(src, 0x06);
            dst[0x24] = src[0x08];
            dst[0x25] = src[0x09];
            dst[0x30] = src[0x0C];
            dst[0x31] = src[0x0D];
            dst += 0x34;
        }
        src += 0x10;
    }

    return src;
}

static u8 *Render_CopyTriPrimData(u8 *dst, u8 *src, int count) {
    int i;
    int side;

    for (i = 0; i < count; i++) {
        for (side = 0; side < 2; side++) {
            dst[0x0C] = src[0x00];
            dst[0x0D] = src[0x01];
            U16_AT(dst, 0x0E) = U16_AT(src, 0x02);
            dst[0x18] = src[0x04];
            dst[0x19] = src[0x05];
            U16_AT(dst, 0x1A) = U16_AT(src, 0x06);
            dst += 0x28;
        }
        src += 0x0C;
    }

    return src;
}

static void Render_InitBoneMatrix(u8 *matrix) {
    U16_AT(matrix, 0x00) = 0x1000;
    U16_AT(matrix, 0x02) = 0;
    U16_AT(matrix, 0x04) = 0;
    U16_AT(matrix, 0x06) = 0;
    U16_AT(matrix, 0x08) = 0x1000;
    U16_AT(matrix, 0x0A) = 0;
    U16_AT(matrix, 0x0C) = 0;
    U16_AT(matrix, 0x0E) = 0;
    U16_AT(matrix, 0x10) = 0x1000;
    U32_AT(matrix, 0x14) = 0;
    U32_AT(matrix, 0x18) = 0;
    U32_AT(matrix, 0x1C) = 0;
}

int Render_SetupEntityPrims(u8 *obj, u8 *block, u8 *prim_base, int arg3, int arg4, int arg5,
                            int arg6, int prim_init_count, u8 **out_prim_data, int setup_prims) {
    u8 *cursor;
    u8 *prim_cursor;
    u8 *prim_data;
    int i;
    int skipped_bones;
    int bone_count;
    int aligned_bytes;

    PTR_AT(obj, 0x00) = block;
    cursor = block + 0x1C;
    PTR_AT(obj, 0x04) = cursor;

    bone_count = block[2];
    cursor += bone_count * 0x0C;
    PTR_AT(obj, 0x08) = cursor;

    cursor += U16_AT(block, 0x06) * 0x08;
    PTR_AT(obj, 0x0C) = cursor;

    cursor += U16_AT(block, 0x06) * 0x04;
    PTR_AT(obj, 0x10) = cursor;

    prim_cursor = prim_base;
    U32_AT(obj, 0x54) = (u32)prim_base;
    U16_AT(obj, 0xBA) = setup_prims;

    if (setup_prims) {
        prim_cursor = Render_InitPrimCodes(prim_cursor, cursor, U16_AT(block, 0x08), 0x34, 0x0C,
                                           0x3C, 0x0B, 0x3E);
        cursor += U16_AT(block, 0x08) * 0x0C;
        prim_cursor = Render_InitPrimCodes(prim_cursor, cursor, U16_AT(block, 0x0A), 0x28, 0x09,
                                           0x34, 0x10, 0x36);
        cursor += U16_AT(block, 0x0A) * 0x0C;
        prim_cursor = Render_InitPrimCodes(prim_cursor, cursor, U16_AT(block, 0x0C), 0x24, 0x08,
                                           0x38, 0x15, 0x3A);
        cursor += U16_AT(block, 0x0C) * 0x0C;
        prim_cursor = Render_InitPrimCodes(prim_cursor, cursor, U16_AT(block, 0x0E), 0x1C, 0x06,
                                           0x30, 0x1A, 0x32);
        cursor += U16_AT(block, 0x0E) * 0x0C;
    } else {
        cursor += (U16_AT(block, 0x08) + U16_AT(block, 0x0A) + U16_AT(block, 0x0C) +
                   U16_AT(block, 0x0E)) *
                  0x0C;
    }

    PTR_AT(obj, 0x14) = cursor;
    cursor += 0x10;
    PTR_AT(obj, 0x18) = cursor;

    cursor += bone_count * 0x10;
    PTR_AT(obj, 0x1C) = cursor;

    cursor += 0x08;
    PTR_AT(obj, 0x20) = cursor;

    aligned_bytes = (U16_AT(block, 0x18) + 3) & ~3;
    cursor += aligned_bytes;

    if (setup_prims) {
        *out_prim_data = cursor;

        prim_data = cursor;
        prim_data = Render_CopyQuadPrimData(prim_base, prim_data, U16_AT(block, 0x08));
        prim_data = Render_CopyTriPrimData(prim_base + U16_AT(block, 0x08) * 0x68, prim_data,
                                           U16_AT(block, 0x0A));

        if (prim_init_count > 0) {
            Render_InitPrimBlock(obj, (s16)arg3, (s16)arg4, (s16)arg5, (s16)arg6);
        }
    }

    U16_AT(obj, 0x70) = U16_AT(PTR_AT(obj, 0x14), 0x06);
    U16_AT(obj, 0x72) = U16_AT(PTR_AT(obj, 0x1C), 0x06);
    U32_AT(obj, 0x80) = (u32)prim_cursor;

    skipped_bones = 0;
    cursor = prim_cursor;
    prim_data = cursor + 0x06;
    for (i = 0; i < bone_count; i++) {
        u8 *node;
        u8 *bone;

        node = PTR_AT(obj, 0x04) + i * 0x0C;
        if (node[4] != 1) {
            skipped_bones++;
            continue;
        }

        bone = PTR_AT(obj, 0x18) + i * 0x10;
        if (S16_AT(bone, 0x0E) < 0) {
            continue;
        }

        U16_AT(prim_data, 0x00) = i;
        U16_AT(prim_data, 0x02) = U16_AT(bone, 0x06);
        U16_AT(prim_data, 0x04) = U16_AT(bone, 0x0E);
        cursor += 0x0C;
        prim_data += 0x0C;
    }

    U32_AT(obj, 0x84) = (u32)cursor;

    for (i = 0; i < bone_count; i++) {
        Render_InitBoneMatrix(cursor);
        cursor += 0x20;
    }

    U16_AT(block, 0x14) = 0;
    S16_AT(obj, 0x2C) = 0;
    S16_AT(obj, 0x2E) = 0;
    S16_AT(obj, 0x30) = 0;
    S16_AT(obj, 0x32) = 1;
    RotMatrix(obj + 0x2C, obj + 0x34);

    Anim_SetInterpRate(obj, 0x32);

    U8_AT(obj, 0x8C) = 0xFF;
    U8_AT(obj, 0x90) = 0x80;
    U8_AT(obj, 0x91) = 0x80;
    U8_AT(obj, 0x92) = 0x80;
    U16_AT(obj, 0x9C) = 0;
    U8_AT(obj, 0x9E) = 1;
    U8_AT(obj, 0x9F) = g_ActiveDrawSlot;

    U16_AT(obj, 0x28) = 0;
    U32_AT(obj, 0x24) = 0;
    U16_AT(obj, 0x2A) = 0;

    U16_AT(block, 0x1A) = U16_AT(block, 0x06) - skipped_bones;
    U16_AT(obj, 0x6E) = U16_AT(PTR_AT(obj, 0x1C), 0x02) + ((s16)U16_AT(obj, 0x72) >> 4);

    U8_AT(obj, 0xA6) = 0;
    U8_AT(obj, 0xA7) = 0;
    U8_AT(obj, 0xAE) = 0;
    U8_AT(obj, 0xAF) = 0;
    U32_AT(obj, 0xB0) = 0;

    return 1;
}
