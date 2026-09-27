/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/task_queue.h"

/* Tentative COMMON declarations preserve small-data metadata for stock
 * maspsx. Storage remains at the existing data/linker addresses. */
QueueNode *D_8009D300;
u32 *D_8009CE00;

void Task_RunQueue(void)
{
    u32 *args[16];
    QueueNode *node;
    u8 *entity;
    u32 header, modes, tail, opcode;
    u32 *values;
    u8 *argument_entity;
    u16 i;

    do {
        node = D_8009D300;
        if (node->flags.word & 0x50) continue;
        entity = D_8009D2F0[0];
        if ((((FieldActor *)entity)->flags & 0x1000) && !(node->flags.half[0] & 0x80)) continue;
        if ((D_8009D1A0[0] & 0x100) && entity != D_8009D254[0] && !(node->flags.half[0] & 0x80)) continue;
        node = D_8009D300;
        if (node->ticks == 0) continue;
        node->ticks--;
        if (node->ticks != 0) continue;
        D_8009CE00 = node->script;
        do {
            header = *D_8009CE00;
            D_8009CE00++;
            values = D_8009CE00 + 1;
            tail = *D_8009CE00;
            modes = header >> 17;
            opcode = header & 0x1FFF;
            header >>= 13;
            header &= 15;
            D_8009CE00 = values + header;
            for (i = 0; i < header; i++) {
                switch (modes & 7) {
                case 0: args[i] = &values[i]; break;
                case 3: args[i] = &D_8009DF70[values[i]]; break;
                case 1:
                    /* Reload after every handler: it may switch actors. */
                    args[i] = (u32 *)((argument_entity = D_8009D2F0[0]) + 0xAC + values[i] * 4);
                    break;
                case 2: args[i] = &D_800A77F0[values[i]]; break;
                case 4: args[i] = &D_800B6A80[values[i]]; break;
                }
                modes >>= 3;
                if (i == 4) modes = tail;
            }
        } while (D_800910A0[opcode](args));
        /* Handlers may replace both the current node and the script cursor. */
        D_8009D300->script = D_8009CE00;
    } while ((D_8009D300 = D_8009D300->next) != 0);
}
