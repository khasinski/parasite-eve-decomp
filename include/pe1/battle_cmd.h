#ifndef PE1_BATTLE_CMD_H
#define PE1_BATTLE_CMD_H

typedef struct SquareMessageEntry {
    /* 0x00 */ unsigned char opcode;
    /* 0x01 */ unsigned char status;
    /* 0x02 */ short slot_idx;
    /* 0x04 */ int arg_04;
    /* 0x08 */ int arg_08;
    /* 0x0C */ union {
        struct {
            unsigned short arg_0C;
            unsigned short arg_0E;
        } halves;
        int word;
    } u;
    /* 0x10 */ int unk_10;
    /* 0x14 */ int arg_14;
    /* 0x18 */ int unk_18;
    /* 0x1C */ int arg_1C;
    /* 0x20 */ int arg_20;
} SquareMessageEntry;

/*
 * FF7's AKAO queue uses the same 0x24-byte entry layout and dispatches by the
 * first byte. PE1 reuses this Square message shape for both AKAO and local
 * battle/menu command queues.
 */
typedef SquareMessageEntry BattleCmdEntry;
typedef SquareMessageEntry AkaoQueueEntry;

#endif
