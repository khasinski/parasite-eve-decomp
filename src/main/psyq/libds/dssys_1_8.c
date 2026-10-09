/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_1.OBJ, part 8 of 11: LIBDS_DSSYS_1_text_EA4. */
#include "common.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds_queue.h"

extern u32 D_8009B624[];

void LIBDS_DSSYS_1_text_EA4(int event, u8 *data) {
    u8 *data_reg;
    DsDecodedEventFlags *state;
    CdRomEventCommandState *eventState;
    u8 value;
    u8 *src;
    int index;

    data_reg = data;
    event &= 0xFF;
    if (event == 5) {
        goto event_five;
    }

    index = D_8009B624[g_CdRomEventCommandState.pendingCommand] - 1;
    if (index < 0) {
        return;
    }
    goto process;

event_five:
    index = 0;

process:
    src = data_reg + index;
    value = src[0];
    asm volatile("" : : "r"(value));
    state = &g_CdRomEventCommandState.command.read.eventFlags;
    asm volatile("" : "=r"(state) : "0"(state));
    state->bit7 = value >> 7;
    state->bit6 = (value >> 6) & 1;
    state->bit5 = (value >> 5) & 1;
    state->bit1 = (value >> 1) & 1;
    eventState = (CdRomEventCommandState *)((u8 *)state -
        (PE1_OFFSETOF(CdRomEventCommandState, command) +
         PE1_OFFSETOF(CdRomCommandState, read) +
         PE1_OFFSETOF(DsReadStatusBlock, eventFlags)));
    eventState->command.eventStatus = value;
    rescpy(eventState->eventResult, data_reg);
}
