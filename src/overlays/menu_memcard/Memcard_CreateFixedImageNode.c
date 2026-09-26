#include "menu_memcard_image.h"

void Memcard_CreateFixedImageNode(void) {
    register s32 kind asm("$3");
    MemcardImageNode *node = D_801D136C;
    register MemcardImageNode *tail;
    MemcardImage *imageData;
    register MemcardImagePreset *preset;
    {
        MemcardImageNode *next = node->next;
        kind = 4;
        D_801D136C = next;
    }
    tail = D_801D137C;
    preset = &D_801D0D8C;
    node->next = 0;
    if (tail) { tail->next = node; D_801D137C = node; }
    else { D_801D137C = node; D_801D1378 = node; }
    node->draw = 0; node->update = 0; node->field14 = 0; node->image = 0;
    node->x = 0; node->y = 0; node->width = 0; node->height = 0;
    node->field1c = 0; node->targetY = 0; node->stepY = 0;
    node->parameter = 0; node->kind = 0; node->field30 = 0;
    __asm__("" : : : "memory");
    imageData = (MemcardImage *)(D_80193254 + D_80193268);
    node->kind = kind;
    node->image = imageData;
    {
        register MemcardImage *image asm("$3") = node->image;
        node->x = preset->x; node->y = preset->y;
        node->width = image->width * 2 / 3;
        image = node->image;
        node->height = image->height;
    }
    {
        register s32 parameter = preset->parameter;
        node->draw = D_80193084;
        node->update = D_8018F7F0;
        node->parameter = parameter;
    }
}
