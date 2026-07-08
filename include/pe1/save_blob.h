#ifndef PE1_SAVE_BLOB_H
#define PE1_SAVE_BLOB_H

typedef unsigned char SaveU8;

typedef struct SaveBytes4 {
    SaveU8 b[4];
} SaveBytes4;

typedef struct SaveBytes8 {
    SaveU8 b[8];
} SaveBytes8;

typedef struct SaveBytes18 {
    SaveU8 b[0x18];
} SaveBytes18;

typedef struct SaveBytes70 {
    SaveU8 b[0x70];
} SaveBytes70;

typedef struct SaveBytes800 {
    SaveU8 b[0x800];
} SaveBytes800;

typedef struct SaveBytes12E4 {
    SaveU8 b[0x12E4];
} SaveBytes12E4;

#endif /* PE1_SAVE_BLOB_H */
