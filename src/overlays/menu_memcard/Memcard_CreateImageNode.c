#include "menu_memcard_image.h"

/* Empty barriers and two pins preserve the original scheduling and frame. */
MemcardImageNode *Memcard_CreateImageNode(s32 kind) {
    s32 workspace[1];
    MemcardImageNode *node = D_801D136C;
    MemcardImageNode *tail = D_801D137C;
    MemcardImage *imageData;
    MemcardImagePreset *preset = &D_801D0D5C[kind];
    __asm__("" : : "m"(workspace[0]));
    D_801D136C = node->next;
    node->next = 0;
    if (tail) { tail->next = node; D_801D137C = node; }
    else { D_801D137C = node; D_801D1378 = node; }
    node->draw = 0; node->update = 0; node->field14 = 0; node->image = 0;
    node->x = 0; node->y = 0; node->width = 0; node->height = 0;
    node->field1c = 0; node->targetY = 0; node->stepY = 0;
    node->parameter = 0; node->kind = 0; node->field30 = 0;
    imageData = (MemcardImage *)(D_80193254 + D_80193258[kind]);
    node->kind = kind;
    node->image = imageData;
    {
        register MemcardImage *image asm("$3") = node->image;
        node->x = preset->x; node->y = preset->y;
        node->width = image->width * 2 / 3;
        image = node->image;
        __asm__("" : : : "memory");
        node->height = image->height;
    }
    {
        register s32 y asm("$3") = node->y;
        s32 parameter;
        __asm__("" : "=r"(y) : "0"(y));
        parameter = preset->parameter;
        node->draw = D_80192FE8; node->update = D_8018F7F0;
        node->targetY = y; node->parameter = parameter;
    }
    node->stepY = -16;
    return node;
}
