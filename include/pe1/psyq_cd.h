#ifndef PE1_PSYQ_CD_H
#define PE1_PSYQ_CD_H

typedef unsigned char u_char;

typedef struct CdlLOC {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

#endif
