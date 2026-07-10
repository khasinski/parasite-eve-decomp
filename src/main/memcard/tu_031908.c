/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */

#include "include_asm.h"
void __maspsx_include_asm_hack_MemCard_UpdateSaveState(void);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_031908", MemCard_UpdateSaveState);

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct SaveSlotRaw {
    u8 active;
    u8 field_01;
    u8 pad_02[0x27];
    u8 name_digit;
    u8 pad_2A[0x1A];
} SaveSlotRaw;

typedef struct SavePortRaw {
    u8 status;
    u8 state;
    u8 field_02;
    u8 selected_slot;
    u8 pad_04;
    u8 write_slot;
    u8 field_06;
    u8 field_07;
    u8 pad_08[2];
    u8 sequence;
    u8 next_state;
    u8 pad_0C[8];
    u16 write_size;
    u16 retry_count;
    u8 pad_18[4];
    SaveSlotRaw slots[15];
} SavePortRaw;

extern char D_800A0ED4[];
extern char D_8009EE70[];
extern char *D_80092224;
extern void *D_800A1854;
extern int D_800A1858;

void MemCard_CloseAll(void);
void Save_BuildHeader(void);
int Square_Vsprintf();
int Save_BuildCardFile(char *path);

void Save_StartWriteSlot(int port, int slot) {
    register SavePortRaw *state asm("$17");
    register int slot_reg asm("$16");
    register int status asm("$18");
    register SaveSlotRaw *slot_state asm("$19");
    int seq;
    int next_state;

    state = (SavePortRaw *)(D_800A0ED4 + (((port << 5) + port) * 4 - port) * 8);
    status = state->status;
    slot_reg = slot;

    if (status == 1) {
        slot_state = &state->slots[slot_reg];
        MemCard_CloseAll();
        Save_BuildHeader();

        seq = state->sequence;
        if (slot_state->active != status) {
            seq++;
        }
        state->selected_slot = slot_reg;
        state->sequence = seq;

        Square_Vsprintf(D_8009EE70, D_80092224, state > (SavePortRaw *)D_800A0ED4,
                        slot_state->name_digit + 0x30, (slot_reg & 0xFF) + 0x41);
        Save_BuildCardFile(D_8009EE70);

        state->write_slot = slot_reg;
        state->field_06 = 0;
        slot_state->field_01 = 0;
        state->write_size = 0x2000;
        state->retry_count = 10;
        state->state = 1;
        state->field_07 = 1;
        D_800A1854 = state;
        D_800A1858 = 0x2000;

        next_state = 4;
        if (slot_state->active == 1) {
            next_state = 0xB;
        }
        state->next_state = next_state;
        slot_state->active = 1;
    }
}
