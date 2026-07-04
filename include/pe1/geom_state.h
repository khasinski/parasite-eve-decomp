#ifndef PE1_GEOM_STATE_H
#define PE1_GEOM_STATE_H

/* g_GeomState (RAM 0x800B1624): the field/room GPU draw context. It is a header
 * (GeomState) holding baked offsets to two parallel entry arrays:
 *   - GeomCtrlEntry[] : 0x10-byte records, base = header->ctrl_offset (+0x10),
 *                       index << 4. The obj/menu/sys control entries.
 *   - GeomEntry[]     : 0x38-byte records, base = header->entry_offset (+0x14)
 *                       or header->entry_offset_1C (+0x1C). The render/mesh
 *                       draw entries.
 *
 * Layout reverse-engineered from the ~28 files that touch g_GeomState; offsets
 * verified at 32-bit pointer width. Several offsets are reused with different
 * widths across files (the entry holds array pointers during build, camera
 * bounds during clamp/render) -- those are modeled as unions so each file keeps
 * its exact load/store width and the byte-match holds. Many g_GeomState files
 * still access it via raw M2C_FIELD() casts; those
 * are intentionally NOT migrated to this header yet (see notes in the files).
 */

typedef signed char    s8;
typedef unsigned char  u8;
typedef short          s16;
typedef unsigned short u16;
typedef int            s32;
typedef unsigned int   u32;

/* 16-byte control entry. base = g_GeomState->ctrl_offset (+0x10), index << 4. */
typedef struct GeomCtrlEntry {            /* 0x10 */
    union {                               /* +0x00  u8 flags vs u32 packed (>>8 = count) */
        u32 packed;
        struct { u8 flags; u8 _b[3]; } b;
    } head;
    u16 field4;                           /* +0x04  (u32 readers use *(u32*)&field4) */
    s16 field6;                           /* +0x06  (read sign-extended in Obj_GetEntryField6) */
    s16 field8;                           /* +0x08 */
    u16 fieldA;                           /* +0x0A */
    s32 slot_offset;                      /* +0x0C  added to entry base -> slot array */
} GeomCtrlEntry;

/* 56-byte render/mesh entry. base = entry_offset (+0x14) or entry_offset_1C (+0x1C). */
typedef struct GeomEntry {                /* 0x38 */
    u8  flags;                            /* +0x00  bits 2,4,8,0x14,0x20 */
    u8  pad01[3];                         /* +0x01 */
    u16 anim_mod_x;                       /* +0x04 */
    u16 anim_mod_y;                       /* +0x06 */
    u16 base_x;                           /* +0x08 */
    u16 base_y;                           /* +0x0A */
    u16 scr_x;                            /* +0x0C */
    u16 scr_y;                            /* +0x0E */
    s16 ot10;                             /* +0x10 */
    s16 ot12;                             /* +0x12 */
    s16 ot14;                             /* +0x14 */
    s16 ot16;                             /* +0x16 */
    u16 disp_x;                           /* +0x18 */
    u16 disp_y;                           /* +0x1A */
    s16 field1C;                          /* +0x1C  scroll/mirror/anim coef */
    s16 field1E;                          /* +0x1E */
    s16 field20;                          /* +0x20 */
    s16 field22;                          /* +0x22 */
    u8  group;                            /* +0x24  == g_GeomGroupSel */
    u8  pad25[1];                         /* +0x25 */
    u16 prim_count;                       /* +0x26 */
    union {                               /* +0x28  pos-array ptr vs two u16 sizes */
        s32 pos_ptr;
        struct { u16 size28; u16 size2A; } sz;
    } u28;
    union {                               /* +0x2C  uv-array ptr vs camera bounds X */
        s32 uv_ptr;
        struct { s16 min_x; s16 max_x; } bx;
    } u2C;
    union {                               /* +0x30  prim ptr vs camera bounds Y */
        void *prim;
        struct { s16 min_y; s16 max_y; } by;
    } u30;
    u8  pad34[4];                         /* +0x34 */
} GeomEntry;

typedef struct GeomState {                /* header */
    u8  pad00[4];                         /* +0x00 */
    u16 entry_count;                      /* +0x04 */
    u16 entry_count06;                    /* +0x06 */
    u8  pad08[8];                         /* +0x08 */
    s32 ctrl_offset;                      /* +0x10  -> GeomCtrlEntry[] */
    s32 entry_offset;                     /* +0x14  -> GeomEntry[] */
    u8  pad18[4];                         /* +0x18 */
    s32 entry_offset_1C;                  /* +0x1C  -> GeomEntry[] (alt base) */
    u8  pad20[6];                         /* +0x20 */
    u16 field26;                          /* +0x26 */
    u8  pad28[4];                         /* +0x28 */
    u16 disp_src_x;                       /* +0x2C */
    u16 disp_src_y;                       /* +0x2E */
    u8  pad30[8];                         /* +0x30 */
    s16 out_disp_x;                       /* +0x38 */
    s16 out_disp_y;                       /* +0x3A */
} GeomState;

extern GeomState * volatile g_GeomState;

#endif /* PE1_GEOM_STATE_H */
