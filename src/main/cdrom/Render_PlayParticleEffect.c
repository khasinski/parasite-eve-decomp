/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_cd.h"

extern CdQueuedCmdSlot D_800A3520;
extern CdQueuedCmdSlot D_800A3530;
extern int D_800B8AB4;

void LIBDS_DSSYS_2_text_170(int command);
void Util_Copy8(u8 *destination, u8 *source);

int Render_PlayParticleEffect(int command, u8 *payload) {
    register int result asm("$2");
    register CdQueuedCmdSlot *slot asm("$3");
    u8 *copy_destination;
    u8 command_byte;

    command_byte = command;
    if (command_byte == 5 && (payload[0] & 0x10)) {
        LIBDS_DSSYS_2_text_170(5);
    }

    switch (command_byte) {
    case 1:
    case 5:
        asm volatile("" : : : "memory");
        slot = &D_800A3520;
        asm volatile("" : "=r"(slot) : "0"(slot));
        copy_destination = slot->payload;
        break;
    case 4:
        slot = &D_800A3530;
        copy_destination = slot->payload;
        break;
    default:
        goto callback;
    }

    slot->state = 1;
    slot->result = command_byte;
    Util_Copy8(copy_destination, payload);

callback:
    asm volatile("" : : : "$2");
    result = D_800B8AB4;
    if (result != 0) {
        result = ((int (*)(u8, u8 *))result)(command_byte, payload);
    }
    return result;
}
