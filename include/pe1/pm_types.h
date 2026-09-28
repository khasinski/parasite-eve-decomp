#ifndef PE1_PM_TYPES_H
#define PE1_PM_TYPES_H

#include "common.h"


/* Both process-manager banks share this prefix. Pm_Exec increments ticks
 * at +4 after calling the command handler; +8 identifies the owning object. */
typedef struct PmSlotHeader {
    u8 state;
    u8 command;
    u8 field02;
    u8 field03;
    u32 ticks;
    void *owner;
} PmSlotHeader;

typedef int (*PmSendCallback)(PmSlotHeader *, int, int, int *, int *, int *);

/* Handler table entries expose both scene-asset and process-manager callbacks. */
typedef struct PmCommand {
    void (*init)(void);
    void (*initialize)(PmSlotHeader *);
    PmSendCallback send;
    int (*start)(void);
    int (*execute)(PmSlotHeader *);
    int (*stop)(void);
    void (*unload)(void);
} PmCommand;

PE1_STATIC_ASSERT(PE1_OFFSETOF(PmCommand, initialize) == 4,
                  pm_command_initializer_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmCommand, send) == 8,
                  pm_command_send_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmCommand, start) == 12,
                  pm_command_start_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmCommand, execute) == 16,
                  pm_command_execute_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmCommand, stop) == 20,
                  pm_command_stop_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmCommand, unload) == 24,
                  pm_command_unload_offset);
PE1_STATIC_ASSERT(sizeof(PmCommand) == 0x1C, pm_command_size);

typedef struct PmPrimarySlot {
    PmSlotHeader header;
    u8 reserved0C[0xA00];
} PmPrimarySlot;

typedef struct PmSecondarySlot {
    PmSlotHeader header;
    u8 reserved0C[0x100];
} PmSecondarySlot;

/* Scene startup partitions the buffer into eleven slots of each kind. */
typedef struct PmSlotBanks {
    PmPrimarySlot primary[11];
    PmSecondarySlot secondary[11];
} PmSlotBanks;

/* Seven adjacent pointer slots used by PM asset commands at 0x800E10A0. */
typedef struct PmAuxiliaryPointerTable {
    void *entries[7];
} PmAuxiliaryPointerTable;

PE1_STATIC_ASSERT(sizeof(PmSlotHeader) == 0x0C, pm_slot_header_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmSlotHeader, ticks) == 4, pm_slot_ticks_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmSlotHeader, owner) == 8, pm_slot_owner_offset);
PE1_STATIC_ASSERT(sizeof(PmPrimarySlot) == 0xA0C, pm_primary_slot_size);
PE1_STATIC_ASSERT(sizeof(PmSecondarySlot) == 0x10C, pm_secondary_slot_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PmSlotBanks, secondary) == 0x6E84,
                  pm_secondary_bank_offset);
PE1_STATIC_ASSERT(sizeof(PmSlotBanks) == 0x7A08, pm_slot_banks_size);
PE1_STATIC_ASSERT(sizeof(PmAuxiliaryPointerTable) == 0x1C,
                  pm_auxiliary_pointer_table_size);


#endif
