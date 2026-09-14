#ifndef PE1_PSYQ_CARD_H
#define PE1_PSYQ_CARD_H

#include "common.h"

/* Observed register window relative to the BIOS patch's incoming v1.
 * Field offsets match the PSX I/O map for a 0x1F800000 base. */
typedef struct CardPatchIoRegisters {
    u8 reserved0000[0x1044];
    volatile u32 serialStatus;
    u8 reserved1048[0x2C];
    volatile u32 interruptMask;
} CardPatchIoRegisters;

PE1_STATIC_ASSERT(PE1_OFFSETOF(CardPatchIoRegisters, serialStatus) == 0x1044,
                  card_patch_serial_status_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CardPatchIoRegisters, interruptMask) == 0x1074,
                  card_patch_interrupt_mask_offset);

/* BIOS continuation slot written by the LIBCARD patch installer. */
extern void *volatile g_CardPatchContinuation;

/* Installed uncached entry: _copy_memcard_patch copies the first patch to
 * 0xDF80, placing CardPatchFunctions at offset 0x2C (0xDFAC). */
extern const u8 D_A000DFAC[];
extern const u32 func_8007E344[];
void func_8007E3B4(void);

#endif
