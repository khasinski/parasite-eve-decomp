#ifndef PE1_FIELD_SFX_H
#define PE1_FIELD_SFX_H

typedef unsigned char FieldSfxU8;
typedef unsigned short FieldSfxU16;

typedef struct FieldSfxQueueEntry {
    FieldSfxU16 field_0;
    FieldSfxU8 field_2;
    FieldSfxU8 field_3;
    int field_4;
    int field_8;
} FieldSfxQueueEntry;

extern FieldSfxU8 g_FieldSfxQueueCount;
extern FieldSfxQueueEntry g_FieldSfxQueue[28];

#endif /* PE1_FIELD_SFX_H */
