#ifndef PE1_RENDER_OBJECT_H
#define PE1_RENDER_OBJECT_H

/* Shared runtime object layout used by the morph and draw paths. */
typedef struct RenderVec3s {
    signed short x;
    signed short y;
    signed short z;
    signed short pad;
} RenderVec3s;

typedef struct RenderObjectPart {
    unsigned short vertex_start;
    unsigned short vertex_count;
    unsigned char visible;
    unsigned char pad_05[7];
} RenderObjectPart;

typedef struct RenderObjectHeader {
    unsigned char pad_00[2];
    unsigned char part_count;
    unsigned char pad_03[0x17];
    unsigned short visible_part_count;
} RenderObjectHeader;

typedef struct RenderObjectEntity {
    /* 0x00 */ RenderObjectHeader *header;
    /* 0x04 */ RenderObjectPart *parts;
    /* 0x08 */ RenderVec3s *vertices;
    /* 0x0C */ unsigned int *vertex_colours;
    /* 0x10 */ unsigned char pad_10[0x08];
    /* 0x18 */ RenderVec3s *bounds_vertices;
    /* 0x1C */ unsigned char pad_1C[0x68];
    /* 0x84 */ int *matrices;
    /* 0x88 */ unsigned char shade;
    /* 0x89 */ unsigned char pad_89[0x13];
    /* 0x9C */ unsigned short flags_9C;
    /* 0x9E */ unsigned char variant_visible;
    /* 0x9F */ unsigned char pad_9F[0x1B];
    /* 0xBA */ signed short draw_count;
} RenderObjectEntity;

#endif
