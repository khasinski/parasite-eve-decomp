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

#include "common.h"
#include "pe1/render_tint.h"

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

/* Animation view of GeomCtrlEntry. Position is a signed 24-bit value
 * with eight fractional bits; the low byte of that word selects the group. */
typedef struct GeomAnimationSlot {
    u8 entry;
    s8 duration;
} GeomAnimationSlot;

typedef struct GeomAnimationControl {
    union { u32 packed; struct { u8 flags; u8 padding[3]; } b; } head;
    unsigned int group : 8;
    signed int position : 24;
    s16 step;
    u16 elapsed;
    s32 slotOffset;
} GeomAnimationControl;

PE1_STATIC_ASSERT(sizeof(GeomAnimationSlot) == 2, geom_animation_slot_size);
PE1_STATIC_ASSERT(sizeof(GeomAnimationControl) == sizeof(GeomCtrlEntry),
                  geom_animation_control_view_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomAnimationControl, step) == 8,
                  geom_animation_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomAnimationControl, elapsed) == 10,
                  geom_animation_elapsed_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomAnimationControl, slotOffset) == 12,
                  geom_animation_slots_offset);

struct RenderTexturePagePacket;

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
    union {                               /* +0x34: second packet-array base */
        struct RenderTexturePagePacket *pagePackets;
        u8 storage[4];
    } u34;
} GeomEntry;

PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomEntry, u34) == 0x34, geom_entry_page_offset);
struct RenderTilePacket;
int Geo_LoadMeshEntry(GeomEntry *entry, struct RenderTilePacket *buffer, void **end);


/* Scrolling view of a 56-byte GeomEntry. Fractions are read as bytes but
 * stored as halfwords, clearing the unused high byte. */
typedef union GeomScrollFraction {
    u16 word;
    u8 byte;
} GeomScrollFraction;

typedef struct GeomScrollEntry {
    u8 flags, padding[3];
    u16 modulusX, modulusY;
    s16 baseX, baseY, x, y;
    u8 reserved10[12];
    s16 speedX, speedY;
    GeomScrollFraction fractionX, fractionY;
    u8 reserved24[20];
} GeomScrollEntry;

/* Observed prefix of the scroll state, not a claim about its full extent.
 * This address-based view spans the interleaved screen-tint block too.
 * The origin pair is also written by script dispatch at 0x800BD028/2A. */
typedef struct GeomScrollCoordinates {
    s16 x, y;
    u16 savedX, savedY;
    u16 screenOffsetX, screenOffsetY;
    u16 startX, startY;
    u16 targetX, targetY;
    u16 elapsed, duration;
    u32 *matrixWords;
    u8 reserved1C[0x10];
    RenderTintState tint;
    u8 reserved70[0x2C];
    s16 originX, originY;
} GeomScrollCoordinates;

typedef struct GeomScrollState {
    u32 flags;
    GeomScrollCoordinates position;
} GeomScrollState;

PE1_STATIC_ASSERT(sizeof(GeomScrollEntry) == 0x38, geom_scroll_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollEntry, x) == 0x0C,
                  geom_scroll_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollEntry, speedX) == 0x1C,
                  geom_scroll_speed_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollEntry, fractionX) == 0x20,
                  geom_scroll_fraction_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollState, position) == 4,
                  geom_scroll_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollCoordinates, originX) == 0x9C,
                  geom_scroll_origin_offset);

PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollState, position.tint) == 0x30,
                  geom_scroll_tint_offset);

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
    s16 clip_min_x;                       /* +0x30 */
    s16 clip_max_x;                       /* +0x32 */
    s16 clip_min_y;                       /* +0x34 */
    s16 clip_max_y;                       /* +0x36 */
    s16 out_disp_x;                       /* +0x38 */
    s16 out_disp_y;                       /* +0x3A */
} GeomState;

extern GeomState * volatile g_GeomState;
extern GeomState * volatile D_800B1624;

extern u8 g_GeomGroupSel;
int Scene_CheckBattleFlag(void);
int Scene_IsBattleMode(void);
int Scene_IsNotBattleMode(void);

/* 52-byte viewport records addressed through header offset 0x1C. */
typedef struct CameraViewport {
    u8 prefix[40];
    u16 width, height;
    s16 minX, maxX, minY, maxY;
} CameraViewport;
PE1_STATIC_ASSERT(sizeof(CameraViewport) == 52, camera_viewport_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CameraViewport, width) == 40,
                  camera_viewport_width_offset);

PE1_STATIC_ASSERT(PE1_OFFSETOF(CameraViewport, minX) == 44,
                  camera_viewport_min_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GeomScrollCoordinates, matrixWords) == 24,
                  camera_matrix_words_offset);
int Render_UpdateScrollPosition(void *position, int duration, int mode);
extern s16 D_800BCFFE;
extern int g_RenderStateFlags;
extern u16 D_800BCF94, D_800BCF96, D_800BCF98, D_800BCF9A;
extern u16 D_800BCF9C, D_800BCF9E, D_800BCFA0, D_800BCFA2;

/* Existing absolute symbols expose overlapping views of the scroll state. */
extern GeomScrollState D_800BCF88;
extern GeomScrollCoordinates D_800BCF8C;
extern u16 D_800BCF8E, D_800BCF90, D_800BCF92;

int Geo_TransformPoint(GeomEntry *entry, int x, int y, int depth);
int Geo_ClipPoint(int x, int y, int z);

#endif /* PE1_GEOM_STATE_H */
