/* MASPSX_FLAGS: -G8 --stack-return-delay */

#include "include_asm.h"

typedef unsigned int u32;
typedef unsigned char u8;

extern u8 D_8009B56C[];
extern void (*D_800A36A8[])(int event, u8 *data);

void CdRom_ProcessEventByte(int event);

void CdRom_ReadyEventDispatch(int event, u8 *data) {
    register int event_reg asm("$16");
    register int event_arg asm("$4");
    register u8 *data_reg asm("$17");
    register u8 *state asm("$3");
    register u32 pending asm("$2");

    data_reg = data;
    event_reg = event & 0xFF;
    asm volatile("" : "=r"(data_reg) : "0"(data_reg));
    asm volatile("" : "=r"(event_reg) : "0"(event_reg));
    CdRom_ProcessEventByte(event_reg);

    state = D_8009B56C;
    asm volatile("" : "=r"(state) : "0"(state));
    if (state[0] & 0x10) {
        ((u32 *)state)[2] = 2;
        ((u32 *)state)[3] = 0xC;
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
