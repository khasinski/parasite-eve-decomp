#ifndef PE1_PSYQ_CD_H
#define PE1_PSYQ_CD_H

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

typedef struct CdlLOC {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

typedef struct DsReadStatusBlock {
    u_int status;
    u_int command;
    u_int sector;
    u_int unk_0C;
    u_int unk_10;
    u_int unk_14;
    u_int unk_18;
    u_int unk_1C;
    u_int syncResult;
    u_int readyResult;
} DsReadStatusBlock;

typedef struct DsDecodedEventFlags {
    u_char bit7;
    u_char bit6;
    u_char bit5;
    u_char bit1;
} DsDecodedEventFlags;

typedef struct DsReadyEventWindow {
    u_char eventStatus;
    u_char unk_01[7];
    u_int status;
    u_int command;
} DsReadyEventWindow;

typedef struct CdQueuedCmdSlot {
    u_int state;
    u_char result;
    u_char payload[8];
    u_char unk_0D[3];
} CdQueuedCmdSlot;

typedef struct CdDsReadQueueEntry {
    u_int active;
    u_char command;
    u_char payload[4];
    u_char unk_09[3];
    u_int arg0C;
    u_int arg10;
    u_int arg14;
} CdDsReadQueueEntry;

#endif
