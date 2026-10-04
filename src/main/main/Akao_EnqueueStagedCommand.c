#include "pe1/akao.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/seq_param.h"

/*
 * Executor behind the Akao_Cmd_* wrappers: turns the staged command in
 * g_AkaoCmd into one or two queue messages. Sample loads (0x10/0x12/0x19)
 * validate the sample header and return its bank id (0 when that bank is
 * already loaded, -1 on a bad header); 0x24 stamps the rolling sequence
 * number; the paired opcodes emit two messages.
 */
int Akao_EnqueueStagedCommand(void) {
    AkaoQueueEntry *msg;
    unsigned short *data;
    unsigned int opcode;
    int *staged;
    int result;
    int param;
    AkaoSequenceCounter sequence;

    result = 0;
    D_8009D268 = 1;
    opcode = g_AkaoCmd.opcode;
    switch (opcode) {
    case 0x10:
    case 0x12:
    case 0x19:
        staged = &g_AkaoCmd.opcode;
        data = g_AkaoCmd.arg0.sample_header;
        if (Spu_ValidateSampleHeader(data) != 0) {
            result = -1;
            break;
        }
        data += 2;
        result = *data;
        data += 2;
        param = *data;
        data += 4;
        if (D_8009D2C8->timing.bank_id != result) {
            Seq_SetParamWithReset(param);
            Akao_AllocMessageSlot(&msg);
            msg->arg0.sample_data = data;
            msg->arg2 = result;
            if (*staged == 0x12)
                msg->arg3 = g_AkaoCmd.arg1;
            msg->opcode.word = *staged;
        } else {
            result = 0;
        }
        break;
    case 0x24:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        sequence = D_8009CDF0;
        msg->arg2 = g_AkaoCmd.arg2;
        D_8009CDF0.value = ((sequence.value + 1) & 0x1FF) + 0x400;
        msg->arg3 = g_AkaoCmd.arg3;
        result = sequence.value;
        msg->sequence = result;
        msg->opcode.word = opcode;
        break;
    case 0xD8:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->opcode.word = 0xD0;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->opcode.word = 0xD4;
        break;
    case 0xD9:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->opcode.word = 0xD1;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->opcode.word = 0xD5;
        break;
    case 0xDA:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->opcode.word = 0xD2;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->opcode.word = 0xD6;
        break;
    case 0x99:
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9B;
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9D;
        break;
    case 0x98:
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9A;
        Akao_AllocMessageSlot(&msg);
        msg->opcode.word = 0x9C;
        break;
    default:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.arg0.value;
        msg->arg1 = g_AkaoCmd.arg1;
        msg->arg2 = g_AkaoCmd.arg2;
        msg->arg3 = g_AkaoCmd.arg3;
        msg->opcode.word = g_AkaoCmd.opcode;
        break;
    }
    D_8009D268 = 0;
    return result;
}
