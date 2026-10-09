#ifndef PE1_WEAPON_LIST_OUTPUT_H
#define PE1_WEAPON_LIST_OUTPUT_H

#include "common.h"

/* Weapon parameters written into the first 0x14 bytes of an action block.
 * The final four bytes of the saved 0x18-byte block are left untouched. */
typedef struct WeaponListOutput {
    u16 attack;
    u16 range;
    u16 unknown04;
    u16 kind;
    u32 unknown08;
    u32 packed;
    u32 effects;
} WeaponListOutput;

PE1_STATIC_ASSERT(sizeof(WeaponListOutput) == 0x14, weapon_list_output_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(WeaponListOutput, packed) == 0xC,
                  weapon_list_output_packed_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(WeaponListOutput, effects) == 0x10,
                  weapon_list_output_effects_offset);

void Inv_BuildWeaponList(int unused, WeaponListOutput *out);

#endif
