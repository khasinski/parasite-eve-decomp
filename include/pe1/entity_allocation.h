#ifndef PE1_ENTITY_ALLOCATION_H
#define PE1_ENTITY_ALLOCATION_H

#include "common.h"

/* Sixteen allocation slots: a run length followed by its scratch-buffer address.
 * A zero address marks a free slot. */
typedef struct EntityAllocationBlock {
    u32 blockCount;
    s32 address;
} EntityAllocationBlock;

PE1_STATIC_ASSERT(sizeof(EntityAllocationBlock) == 8, entity_allocation_block_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EntityAllocationBlock, address) == 4,
                  entity_allocation_block_address);
extern EntityAllocationBlock D_800A7620[16];

#endif
