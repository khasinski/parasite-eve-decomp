#include "pe1/akao.h"
#include "pe1/akao/init_state.h"
#include "pe1/akao/voice_state.h"
typedef struct { int value; } Rec;
extern Rec R_Arg0, R_Arg1, R_Arg2, R_Arg3, R_Op;
extern struct { unsigned short *value; } R_Data;

/* Turns the staged command (g_AkaoCmdOpcode plus up to four arguments) into
 * one or two AKAO queue messages. Returns the loaded bank id for the sample
 * load opcodes, -1 when the sample header is invalid, otherwise 0. */
int Akao_EnqueueStagedCommand(void)
{
    AkaoStagedMessage *msg;
    unsigned short *data;
    unsigned int opcode;
    int *staged;
    int result;
    int param;

    result = 0;
    D_8009D268 = 1;
    opcode = g_AkaoCmdOpcode;
    switch (opcode) {
    case 0x10:
    case 0x12:
    case 0x19:
        data = R_Data.value;
        staged = &R_Op.value;
        if (Spu_ValidateSampleHeader(data) != 0) {
            result = -1;
            break;
        }
        data += 2;
        result = *data;
        data += 2;
        param = *data;
        data += 4;
        if (g_AkaoCurTrack->bank_id != result) {
            Seq_SetParamWithReset(param);
            Akao_AllocStagedMessage(&msg);
            msg->arg0.sampleData = data;
            msg->arg2 = result;
            if (*staged == 0x12) {
                msg->arg3 = R_Arg1.value;
            }
            msg->opcode = *staged;
        } else {
            result = 0;
        }
        break;
    case 0x24:
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->arg1 = R_Arg1.value;
        result = D_8009CDF0;
        msg->arg2 = R_Arg2.value;
        D_8009CDF0 = ((result + 1) & 0x1FF) + 0x400;
        msg->sequence = result;
        msg->opcode = opcode;
        msg->arg3 = R_Arg3.value;
        break;
    case 0xD8:
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->opcode = 0xD0;
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->opcode = 0xD4;
        break;
    case 0xD9:
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->opcode = 0xD1;
        msg->arg1 = R_Arg1.value;
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->opcode = 0xD5;
        msg->arg1 = R_Arg1.value;
        break;
    case 0xDA:
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->arg1 = R_Arg1.value;
        msg->opcode = 0xD2;
        msg->arg2 = R_Arg2.value;
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->arg1 = R_Arg1.value;
        msg->opcode = 0xD6;
        msg->arg2 = R_Arg2.value;
        break;
    case 0x99:
        Akao_AllocStagedMessage(&msg);
        msg->opcode = 0x9B;
        Akao_AllocStagedMessage(&msg);
        msg->opcode = 0x9D;
        break;
    case 0x98:
        Akao_AllocStagedMessage(&msg);
        msg->opcode = 0x9A;
        Akao_AllocStagedMessage(&msg);
        msg->opcode = 0x9C;
        break;
    default:
        Akao_AllocStagedMessage(&msg);
        msg->arg0.value = R_Arg0.value;
        msg->arg1 = R_Arg1.value;
        msg->arg2 = R_Arg2.value;
        msg->arg3 = R_Arg3.value;
        msg->opcode = R_Op.value;
        break;
    }
    D_8009D268 = 0;
    return result;
}
