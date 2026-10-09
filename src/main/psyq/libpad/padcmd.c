/* ASSEMBLER: GNU */
/* Psy-Q LIBPAD PADCMD.OBJ: _padSetAct, _padSetCmd, _padSendAtLoadInfo, _padRecvAtLoadInfo, _padGetActSize, _padLoadActInfo, _padSetActAlign, _padSetMainMode, _padCmdParaMode and the command emitters. */
#include "pe1/card_obj.h"
#include "common.h"
#include "../../../../tools/m2c/m2c_macros.h"

void CardObj_EmitCommand45(CardObj *arg0);
void CardObj_EmitCommand4C(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand47(CardObj *arg0, unsigned char arg1);

void _padSetAct(CardObj *arg0, int arg1, unsigned char arg2)
{
    arg0->payload_28 = (unsigned char *)arg1;
    arg0->payload_28_len = arg2;
}

void _padSetCmd(CardObj *arg0, unsigned char arg1, int arg2, unsigned char arg3)
{
    arg0->command = arg1;
    arg0->payload_2c = (unsigned char *)arg2;
    arg0->payload_2c_len = arg3;
}

void _padSendAtLoadInfo(CardObj *arg0)
{
    int state = arg0->field_46;

    switch (state) {
    case 2:
        CardObj_EmitCommand45(arg0);
        break;
    case 3:
        CardObj_EmitCommand4C(arg0, arg0->field_e4);
        break;
    case 4:
        CardObj_EmitCommand47(arg0, arg0->field_47);
        break;
    }
}

int _padGetActSize(CardObj *obj);
int _padLoadActInfo(CardObj *obj, unsigned char *dst);

#define CARD_RESPONSE(obj) \
    (*(unsigned char * volatile *)&(obj)->response_3c)

int _padRecvAtLoadInfo(CardObj *obj) {
    unsigned int chunk;
    register unsigned int next asm("$2");
    unsigned int responseValue;
    switch (obj->field_46) {
    case 2:
        obj->field_e3 = CARD_RESPONSE(obj)[3];
        obj->field_e4 = CARD_RESPONSE(obj)[4];
        obj->field_e6 = 0;
        obj->field_e9 = CARD_RESPONSE(obj)[5];
        obj->field_ea = CARD_RESPONSE(obj)[6];
        obj->field_ec = 0;
        break;

    case 3:
        responseValue = obj->response_3c[4];
        next = obj->response_3c[5];
        obj->field_47 = 0;
        obj->field_e6 = (responseValue << 8) + next;
        break;

    case 4:
        chunk = obj->field_ec;
        next = obj->field_47;
        responseValue = obj->response_3c[4];
        next++;
        obj->field_47 = next;
        chunk += 8;
        chunk += (responseValue + 3) & 0x1FC;
        obj->field_ec = chunk;
        if ((next & 0xFF) < obj->field_ea) {
return_zero:
            return 0;
        }

        if (_padGetActSize(obj) >= 0x81) {
            obj->field_46 = 0xFE;
            obj->field_49 = 2;
            goto return_zero;
        }

        obj->field_46 = 0xFF;
        _padLoadActInfo(obj, (unsigned char *)obj + 0x63);
        obj->field_46 = 2;
        goto return_zero;
    }

    return 1;
}

int _padGetActSize(CardObj *arg0) {
    int first;
    int second;
    int responseBytes;
    int base;
    int raw_first;
    int raw_second;

    raw_first = arg0->field_e3;
    raw_second = arg0->field_e9;
    base = arg0->field_ec;

    first = raw_first + 1;
    first >>= 1;
    first <<= 2;

    responseBytes = (raw_second << 2) + raw_second;
    second = (responseBytes + 3) & 0xFFC;
    second += 4;

    first += second;
    return first + base;
}

void CardObj_EmitReadTransferCommand(CardObj *obj);
int LIBPAD_PADCMD_text_3A0(CardObj *obj);

int _padLoadActInfo(CardObj *obj, unsigned char *buffer) {
    int cursor;
    int result;
    int state;
    register int (*processFn)(CardObj *);
    int rowCount;
    int columnCount;

    if (buffer == 0) {
        goto return_zero;
    }
    if (obj->capabilities != 0) {
        return 0;
    }
    if (g_MemCardIsTransferActiveFn() == 0) {
        goto initialize;
    }

return_zero:
    return 0;

initialize:
    result = 1;
    asm volatile("" : "=r"(result) : "0"(result));
    state = 4;
    cursor = ((int)buffer + 3) >> 2;
    obj->field_49 = state;
    state = 1;
    obj->field_46 = state;
    obj->fn_14 = (void (*)(void *))CardObj_EmitReadTransferCommand;
    asm volatile("" ::: "memory");
    rowCount = obj->field_e3;
        processFn = LIBPAD_PADCMD_text_3A0;
    obj->fn_18 = processFn;
    asm volatile("" ::: "memory");
    columnCount = obj->field_e9;
    asm volatile("" : "=r"(rowCount), "=r"(columnCount), "=r"(result) : "0"(rowCount), "1"(columnCount), "2"(result));

    cursor <<= 2;
    obj->modeTable = (u16 *)cursor;
    obj->field_47 = 0;
    asm volatile("" ::: "memory");
    cursor += ((rowCount + 1) >> 1) * 4;
    obj->capabilities = (PadCapabilityRecord *)cursor;
    cursor += (columnCount * 5 + 3) & 0xFFC;
    obj->combinations = (PadCombinationRecord *)cursor;
    return result;
}

void CardObj_EmitCommand46(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand4B(CardObj *arg0);

void CardObj_EmitReadTransferCommand(CardObj *arg0) {
    int state = arg0->field_46;

    switch (state) {
    case 2:
        CardObj_EmitCommand4C(arg0, arg0->field_47);
        break;
    case 3:
        CardObj_EmitCommand46(arg0, arg0->field_47);
        break;
    case 4:
        if (arg0->field_48 == 0) {
            CardObj_EmitCommand47(arg0, arg0->field_47);
        } else {
            CardObj_EmitCommand4B(arg0);
        }
        break;
    }
}

/* Psy-Q LIBPAD/PADCMD.OBJ private text_3A0.
 * Provenance: configs/USA/psyq_provenance.json (LIBPAD PADCMD). */
extern u8 *D_800A5AD0;
int LIBPAD_PADCMD_text_3A0(CardObj *inPort) {
    register CardObj *port = inPort;
    register int result asm("$2");
    switch (port->field_46) {
    case 2:
        port->modeTable[port->field_47] =
            port->response_3c[5] + (port->response_3c[4] << 8);
        port->field_47++;
        if (port->field_47 >= port->field_e3) {
            port->field_47 = 0;
            goto complete;
        }
        result = 0;
        break;
    case 3: {
        PadCapabilityRecord *record =
            port->capabilities + port->field_47;
        record->protocol[0] = port->response_3c[4];
        record->protocol[1] = port->response_3c[5] & 127;
        record->payloadBytes = port->response_3c[6];
        record->activationCost = port->response_3c[7];
        {
            register int high = port->response_3c[5];
            record->high_bit = high >> 7;
        }
        port->field_47++;
        if (port->field_47 >= port->field_e9) {
            port->field_47 = 0;
            port->field_48 = 0;
            goto complete;
        }
        result = 0;
        break;
    }
    case 4: {
        PadCombinationRecord *record = port->combinations + port->field_47;
        register u8 *source, *base;
        register int bytes asm("$4");
        unsigned offset;
        if (port->field_48 == 0) {
            {
                register int length = port->response_3c[4];
                bytes = 3;
                port->field_48 = length;
            }
            record->length = port->field_48;
            {
                register u8 *response = port->response_3c;
                register int index = port->field_47;
                source = response + 5;
                if (index == 0) {
                    base = (u8 *)port->combinations;
                    offset = port->field_ea * 8;
                } else {
                    base = record[-1].data;
                    offset = (record[-1].length + 3) & 0x1fc;
                }
            }
            base = base + offset;
            record->data = base;
            D_800A5AD0 = base;
        } else {
            register u8 *response = port->response_3c;
            bytes = 6;
            source = response + 2;
        }
        bytes--;
        if (bytes != -1) {
            register u8 **destination = &D_800A5AD0;
            do {
                register int remaining = port->field_48;
                bytes--;
                if (!remaining)
                    goto exhausted;
                {
                    register u8 *dst = *destination;
                    register int value = *source++;
                    asm("" : "=r"(source) : "0"(source));
                    *dst = value;
                    *destination = dst + 1;
                }
                port->field_48--;
            } while (bytes != -1);
        }

        if (port->field_48 == 0)
            goto exhausted;
    zero:
        asm("" ::: "memory");
        result = 0;
        break;
    exhausted:
        {
            port->field_47++;
            if (port->field_47 >= port->field_ea) {
                port->field_49 = 6;
                port->field_46 = 254;
                result = 0;
                break;
            }
            port->field_48 = 0;
        }
        goto zero;
    }
    default:
        result = 1;
        break;
    }
    return result;
complete:
    result = 1;
    return result;
}

extern int (*D_8009B740)(CardObj *obj);

void func_80083C20(void *obj);
int func_80083C3C(CardObj *obj);

int _padSetActAlign(CardObj *obj, int command) {
    register int result asm("$2");
    int active;

    result = D_8009B740(obj);
    if (result != 0) {
        result = 0;
    } else {
        active = 1;
        obj->field_46 = active;
        obj->fn_14 = func_80083C20;
        obj->field_20 = command;
        obj->fn_18 = func_80083C3C;
        result = 1;
    }
    return result;
}

void CardObj_EmitCommand4D(CardObj *arg0) {
    int value = arg0->field_20;

    arg0->command = 0x4D;
    arg0->payload_2c_len = 6;
    arg0->payload_2c = (unsigned char *)value;
}

#define NULL ((void *)0)
s32 Render_CheckParticleBounds(CardObj *arg0) {
    s32 i;
    s32 ff;
    s32 offset;
    u8 *cursor;
    s32 matched;
    register s32 n asm("$3");
    s32 needed;
    register u8 *out asm("$6");
    register s32 limit asm("$2");

    i = 0;
    if (arg0->field_e9 != 0) {
        ff = 0xFF;
        offset = 0;
        do {
            cursor = (u8 *)arg0->field_20;
            matched = 0;
            n = 5;
            do {
                if (*cursor++ == i) {
                    matched += 1;
                }
                n -= 1;
            } while (n >= 0);
            needed = ((PadCapabilityRecord *)(offset + (u32)arg0->capabilities))->payloadBytes;
            cursor = (u8 *)arg0->field_20;
            n = 0;
            if (needed == 0) {
                needed = 1;
            }
            out = (u8 *)arg0;
            do {
                if (*cursor++ == i) {
                    if (matched < needed) {
                        out[PE1_OFFSETOF(CardObj, field_5d)] = ff;
                        matched -= 1;
                    } else {
                        out[PE1_OFFSETOF(CardObj, field_5d)] = i;
                    }
                }
                n += 1;
                out += 1;
            } while (n < 6);
            limit = arg0->field_e9;
            i += 1;
            offset += 5;
        } while (i < limit);
    }
    arg0->field_46 = 0xFE;
    return 0;
}

void CardObj_EmitReadIdCommand(CardObj *obj);
int CardObj_CheckAbortOrDispatch(CardObj *obj);

int _padSetMainMode(CardObj *obj, int byte1, int byte2) {
    register int compareByte asm("$19");
    register int flag asm("$3");
    register int result;

    compareByte = byte1;
    result = g_MemCardIsTransferActiveFn(obj);
    if (result != 0) {
        return 0;
    }

    result = 1;
    asm("" : "=r"(result) : "0"(result));
    flag = 1;
    obj->field_46 = flag;
    obj->fn_14 = (void (*)(void *))CardObj_EmitReadIdCommand;
    obj->fn_18 = CardObj_CheckAbortOrDispatch;
    obj->field_51 = byte1;
    obj->field_52 = byte2;
    flag = (compareByte & 0xFF) ^ obj->field_e4;
    flag = (unsigned int)flag < 1;
    obj->field_53 = flag;
    return result;
}

void CardObj_EmitReadIdCommand(CardObj *arg0) {
    int state = arg0->field_46;

    switch (state) {
    case 2:
        arg0->command = 0x44;
        arg0->payload_2c = (unsigned char *)arg0 + 0x51;
        arg0->payload_2c_len = state;
        break;
    case 3:
        arg0->command = 0x4D;
        arg0->payload_2c = (unsigned char *)arg0 + 0x5D;
        arg0->payload_2c_len = 6;
        break;
    }
}
extern int (*D_8009B728)(void *);

int CardObj_CheckAbortOrDispatch(CardObj *arg0) {
    if (*((unsigned char *)arg0 + 0x53) != 0) {
        if (arg0->field_46 == 2) {
            return 1;
        }

        arg0->field_46 = 0xFE;
        return 0;
    }

    D_8009B728(arg0);
    return 0;
}
void _padCmdParaMode(CardObj *obj, unsigned char value) {
    obj->command = 0x43;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand45(CardObj *obj) {
    obj->command = 0x45;
    obj->payload_2c = 0;
    obj->payload_2c_len = 0;
}
void CardObj_EmitCommand4C(CardObj *obj, unsigned char value) {
    obj->command = 0x4C;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand46(CardObj *obj, unsigned char value) {
    obj->command = 0x46;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand47(CardObj *obj, unsigned char value) {
    obj->command = 0x47;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand4B(CardObj *obj) {
    obj->command = 0x4B;
    obj->payload_2c = 0;
    obj->payload_2c_len = 0;
}
