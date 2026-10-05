#ifndef PE1_FIELD_TEXTURED_CHAIN_NODE_H
#define PE1_FIELD_TEXTURED_CHAIN_NODE_H

#include "common.h"
#include "pe1/gte_types.h"

/* One 0x44-byte node is generated as a tilted matrix link and rendered as a
 * textured strip segment. The edge vectors occupy the same slots in both
 * views; the strip renderer ignores the tilt and matrix fields. */
typedef struct FieldTexturedChainNode {
    /* 0x00 */ u8 visible;
    /* 0x01 */ u8 pad01[2];
    /* 0x03 */ u8 cellStep;
    /* 0x04 */ s16 brightness;
    /* 0x06 */ u8 pad06[4];
    /* 0x0A */ s16 tiltX;
    /* 0x0C */ s16 tiltY;
    /* 0x0E */ u8 pad0E[2];
    /* 0x10 */ u8 rgb[4];
    /* 0x14 */ GteShortVector edgeA;
    /* 0x1C */ GteShortVector edgeB;
    /* 0x24 */ GteMatrix matrix;
} FieldTexturedChainNode;

typedef FieldTexturedChainNode FieldChainLink;
typedef FieldTexturedChainNode FieldStripNode;

PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, cellStep) == 3,
                  field_textured_chain_cell_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, brightness) == 4,
                  field_textured_chain_brightness_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, tiltX) == 0x0A,
                  field_textured_chain_tilt_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, tiltY) == 0x0C,
                  field_textured_chain_tilt_y_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, rgb) == 0x10,
                  field_textured_chain_rgb_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, edgeA) == 0x14,
                  field_textured_chain_edge_a_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, edgeB) == 0x1C,
                  field_textured_chain_edge_b_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldTexturedChainNode, matrix) == 0x24,
                  field_textured_chain_matrix_offset);
PE1_STATIC_ASSERT(sizeof(FieldTexturedChainNode) == 0x44,
                  field_textured_chain_node_size);

#endif /* PE1_FIELD_TEXTURED_CHAIN_NODE_H */
