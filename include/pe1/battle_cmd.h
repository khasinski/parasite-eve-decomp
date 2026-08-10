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
typedef SquareMessageEntry AkaoQueueEntry;

typedef union BattleCmdHeader {
    /* Command code is written and compared as a full word by the battle queue. */
    unsigned int word;
    struct {
        unsigned char opcode;
        unsigned char status;
        short slot_index;
    } bytes;
} BattleCmdHeader;

typedef union BattleCmdPayload {
    struct {
        int arg_04;
        int arg_08;
        int arg_0C;
        int arg_10;
        int arg_14;
        int arg_18;
        int arg_1C;
        int arg_20;
    } generic;
    /* opcode 0: restore an inventory slot removed by a pending command. */
    struct {
        int item_id;
        int slot_index;
        unsigned char reserved0C[0x18];
    } inventory_restore;
    /* opcode 1: pending ammunition consumption for an inventory item. */
    struct {
        int item_index;
        int amount;
        unsigned char reserved0C[0x18];
    } ammo_spend;
    /* opcodes 2 and 3: restore the previous equipped item. */
    struct {
        int item_data;
        unsigned char reserved08[0x1C];
    } equip_restore;
    /* opcode 4: restore ammo values across three item-data records. */
    struct {
        int item_data0;
        int item_data1;
        int item_data2;
        int reserved10;
        int ammo0;
        int reserved18;
        int ammo1;
        int ammo2;
    } ammo_restore;
} BattleCmdPayload;

typedef struct BattleCmdEntry {
    /* 0x00 */ BattleCmdHeader header;
    /* 0x04 */ BattleCmdPayload payload;
} BattleCmdEntry;

#define BATTLE_CMD_OFFSETOF(type, member) \
    ((unsigned int)&(((type *)0)->member))
#define BATTLE_CMD_STATIC_ASSERT(condition, name) \
    typedef char battle_cmd_static_assert_##name[(condition) ? 1 : -1]

BATTLE_CMD_STATIC_ASSERT(sizeof(BattleCmdHeader) == 0x04, header_size);
BATTLE_CMD_STATIC_ASSERT(sizeof(BattleCmdPayload) == 0x20, payload_size);
BATTLE_CMD_STATIC_ASSERT(
    BATTLE_CMD_OFFSETOF(BattleCmdEntry, payload.ammo_spend.amount) == 0x08,
    ammo_amount_offset);
BATTLE_CMD_STATIC_ASSERT(
    BATTLE_CMD_OFFSETOF(BattleCmdEntry, payload.ammo_restore.ammo2) == 0x20,
    restore_tail_offset);
BATTLE_CMD_STATIC_ASSERT(sizeof(BattleCmdEntry) == 0x24, entry_size);

#undef BATTLE_CMD_STATIC_ASSERT
#undef BATTLE_CMD_OFFSETOF

#endif
