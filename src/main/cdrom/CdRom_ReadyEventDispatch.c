/* GCC_VERSION: 2.8.1 */
#include "common.h"

#include "include_asm.h"
#include "pe1/psyq_cd.h"

extern DsReadyEventWindow g_DsReadyEventWindow asm("D_8009B56C");
extern void (*D_800A36A8[])(int event, u8 *data);

void CdRom_ProcessEventByte(int event);

void CdRom_ReadyEventDispatch(int event, u8 *data) {
    int event_reg;
    register int event_arg asm("$4");
    u8 *data_reg;
    DsReadyEventWindow *state;
    u32 pending;
    data_reg = data;
    event_reg = event & 0xFF;
            CdRom_ProcessEventByte(event_reg);

    state = &g_DsReadyEventWindow;
    asm volatile("" : "=r"(state) : "0"(state));
    if (state->eventStatus & 0x10) {
        state->status = 2;
        state->command = 0xC;
    }

    if (D_800A36A8[0] != 0) {
        pending = ((u32 *)state)[-6];
        if (pending != 0) {
            event_arg = event_reg;
            asm volatile("" : "=r"(event_arg) : "0"(event_arg));
            asm volatile(
                "lui $2,%%hi(D_800A36A8)\n"
                "lw $2,%%lo(D_800A36A8)($2)\n"
                "nop\n"
                "jalr $2\n"
                "addu $5,%0,$0"
                :
                : "r"(data_reg)
                : "$2", "$4", "$5", "$31", "memory");
        }
    }
}
