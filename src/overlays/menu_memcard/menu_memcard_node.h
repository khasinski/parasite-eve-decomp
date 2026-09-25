#ifndef MENU_MEMCARD_NODE_H
#define MENU_MEMCARD_NODE_H

#include "common.h"

typedef struct MenuMemcardListNode {
    struct MenuMemcardListNode *next;
    u8 reserved04[0x08];
    void *payload;
    u8 reserved10[0x0C];
    s32 fadeLevel;
    u8 reserved20[0x0C];
    s32 kind;
} MenuMemcardListNode;

PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardListNode, fadeLevel) == 0x1C,
                  menu_memcard_node_fade_level_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardListNode, payload) == 0x0C,
                  menu_memcard_node_payload_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardListNode, kind) == 0x2C,
                  menu_memcard_node_kind_offset);

extern MenuMemcardListNode *D_801D1370;

#endif
