#ifndef MENU_MEMCARD_IMAGE_H
#define MENU_MEMCARD_IMAGE_H

#include "common.h"
typedef struct MemcardImage { u8 header[16]; s16 width, height; } MemcardImage;
typedef struct MemcardImageNode {
    struct MemcardImageNode *next;
    s16 x, y, width, height;
    void *draw;
    void *update;
    void *field14;
    MemcardImage *volatile image;
    s32 field1c, targetY, stepY, parameter, kind, field30;
} MemcardImageNode;
typedef struct MemcardImagePreset { s32 x, y, parameter; } MemcardImagePreset;
extern MemcardImageNode *D_801D136C, *D_801D137C, *D_801D1378;
extern MemcardImagePreset D_801D0D5C[];
extern MemcardImagePreset D_801D0D8C;
extern s32 D_80193268;
extern u8 D_80193084[];
extern s32 D_80193258[];
extern u8 D_80193254[], D_80192FE8[], D_8018F7F0[];

MemcardImageNode *Memcard_CreateImageNode(s32 kind);
void Memcard_CreateFixedImageNode(void);
void Memcard_CreateImageGroup(void);
extern MemcardImagePreset D_801D0DB0, D_801D0DA4;
extern s32 D_80193274, D_80193270;
extern u8 D_801930D8[], D_8018F958[], D_80192F98[];

void Memcard_BlendImage(MemcardImageNode *node, u8 *destination, s32 padding);

void Memcard_BlendRotatedImage(MemcardImageNode *node, u8 *destination, s32 padding, s32 stride);

#endif
