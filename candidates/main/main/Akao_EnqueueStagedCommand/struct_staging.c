/* Akao_EnqueueStagedCommand scratch candidate: lev 16 (retail 242 words, mine 242).
 * The declarations below are the scratch view used for scoring (see README):
 * the staging globals as ONE struct and a queue message with a word opcode.
 * Landing it needs those two type changes in the shared headers. */
typedef struct { unsigned short pad[0x2A]; unsigned short bank_id; } Bank;
extern Bank *D_8009D2C8;
int Spu_ValidateSampleHeader(void *header);
void Seq_SetParamWithReset(unsigned int param);
extern int D_8009D268;
extern struct { int value; } D_8009CDF0;
typedef union { int value; unsigned short *data; } StagedArg;
/* Staging area at 0x800BCD80 (today five scalars g_AkaoCmdOpcode..Arg3). */
typedef struct { int opcode; StagedArg args[4]; } StagedCmd;
extern StagedCmd g_AkaoCmd;
typedef struct { int opcode; StagedArg arg0; int arg1, arg2, arg3, sequence; } Msg;
void Akao_AllocMessageSlot(Msg **out);

int Akao_EnqueueStagedCommand(void)
{
    Msg *msg;
    unsigned short *data;
    unsigned int opcode;
    int *staged;
    int result;
    int param;

    result = 0;
    D_8009D268 = 1;
    opcode = g_AkaoCmd.opcode;
    switch (opcode) {
    case 0x10:
    case 0x12:
    case 0x19:
        staged = &g_AkaoCmd.opcode;
        data = g_AkaoCmd.args[0].data;
        if (Spu_ValidateSampleHeader(data) != 0) {
            result = -1;
            break;
        }
        data += 2;
        result = *data;
        data += 2;
        param = *data;
        data += 4;
        if (D_8009D2C8->bank_id != result) {
            Seq_SetParamWithReset(param);
            Akao_AllocMessageSlot(&msg);
            msg->arg0.data = data;
            msg->arg2 = result;
            if (*staged == 0x12)
                msg->arg3 = g_AkaoCmd.args[1].value;
            msg->opcode = *staged;
        } else {
            result = 0;
        }
        break;
    case 0x24:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->arg1 = g_AkaoCmd.args[1].value;
        msg->arg2 = g_AkaoCmd.args[2].value;
        msg->opcode = opcode;
        result = D_8009CDF0.value;
        D_8009CDF0.value = ((result + 1) & 0x1FF) + 0x400;
        msg->sequence = result;
        msg->arg3 = g_AkaoCmd.args[3].value;
        break;
    case 0xD8:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->opcode = 0xD0;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->opcode = 0xD4;
        break;
    case 0xD9:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->arg1 = g_AkaoCmd.args[1].value;
        msg->opcode = 0xD1;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->arg1 = g_AkaoCmd.args[1].value;
        msg->opcode = 0xD5;
        break;
    case 0xDA:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->arg1 = g_AkaoCmd.args[1].value;
        msg->arg2 = g_AkaoCmd.args[2].value;
        msg->opcode = 0xD2;
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->arg1 = g_AkaoCmd.args[1].value;
        msg->arg2 = g_AkaoCmd.args[2].value;
        msg->opcode = 0xD6;
        break;
    case 0x99:
        Akao_AllocMessageSlot(&msg);
        msg->opcode = 0x9B;
        Akao_AllocMessageSlot(&msg);
        msg->opcode = 0x9D;
        break;
    case 0x98:
        Akao_AllocMessageSlot(&msg);
        msg->opcode = 0x9A;
        Akao_AllocMessageSlot(&msg);
        msg->opcode = 0x9C;
        break;
    default:
        Akao_AllocMessageSlot(&msg);
        msg->arg0.value = g_AkaoCmd.args[0].value;
        msg->arg1 = g_AkaoCmd.args[1].value;
        msg->arg2 = g_AkaoCmd.args[2].value;
        msg->arg3 = g_AkaoCmd.args[3].value;
        msg->opcode = g_AkaoCmd.opcode;
        break;
    }
    D_8009D268 = 0;
    return result;
}
