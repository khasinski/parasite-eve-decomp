/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-cse-follow-jumps */
/* PSY-Q LIBPAD PADIF: MemCard_ReadByteWithCallbackValue,
 * CardObj_WriteCommandByte, MemCard_WriteDataAndGetSize,
 * MemCard_WriteByteWithAckCheck, LIBPAD_PADIF_text_26C. */

#include "common.h"
#include "pe1/card_obj.h"
#include "pe1/psyq_pad_main.h"

#include "padif_externs.h"

int MemCard_ReadByteWithCallbackValue(CardObj *port) {
    g_MemCardDispatchResult = g_MemCardStateDispatchFn(port);
    return _padSioRW(port, -2);
}

int MemCard_WriteByte(CardObj *obj, int value);

void CardObj_WriteCommandByte(CardObj *obj) {
    if (g_PadActivePortIndex == g_MemCardPort1Present && D_8009B228 != 0) {
        D_8009B21C();
        D_8009B218();
    }

    if (g_MemCardDispatchResult != 0) {
        g_MemCardStateDispatchFn(obj->subPorts);
        g_MemCardStateDispatchFn(obj->subPorts + 1);
    }

    if (obj->command == 0) {
        MemCard_WriteByte(obj, 0x42);
    } else {
        MemCard_WriteByte(obj, obj->command);
    }
}

int MemCard_WriteDataAndGetSize(CardObj *obj) {
    int result;
    int blockCount;
    int value;

    if (g_MemCardDispatchResult != 0) {
        g_MemCardStateDispatchFn(obj->subPorts + 2);
        g_MemCardStateDispatchFn(obj->subPorts + 3);
    }

    value = 0;
    if (obj->command == 0) {
        value = g_MemCardCommandByte;
    }

    result = MemCard_WriteByte(obj, value);
    if (result < 0) {
        return result;
    }
    if ((result & 0xF0) == 0) {
        return -9;
    }

    blockCount = (result & 0xF) << 1;
    g_MemCardWriteBlockCount = blockCount;
    if (blockCount == 0) {
        g_MemCardWriteBlockCount = 0x20;
    }
    return 0;
}

int MemCard_WriteByteWithAckCheck(CardObj *obj) {
    register CardObj *card asm("$16");
    CardObj *call_obj;
    int needs_ack;
    int value;
    register int result asm("$3");
    int ret;
    register u32 zero asm("$0");
    card = obj;
    needs_ack = 0;
    if ((card->response_3c[0] >> 4) == 8) {
        needs_ack = card->command < 1;
    }

    asm volatile("" : : : "$4");
    call_obj = card;
    value = g_MemCardObjReadPayloadByteFn(call_obj, needs_ack);
    call_obj = card;
    result = MemCard_WriteByte(call_obj, value & 0xFF);
    /* Match debt: architectural zero keeps the result copy in the first
     * branch delay slot; the barriers preserve the shared return path. */
        ret = 90;
    asm volatile("" : "=r"(ret) : "0"(ret));
    if (result == ret) {
        ret = result + zero;
        return ret;
    }
    ret = result + zero;
    asm volatile("" : "=r"(result), "=r"(ret) : "0"(result), "1"(ret));
    if (result != 0) {
    /* Retail returns -9 for positive values other than 90, and preserves
     * negative values. The BGEZ delay slot assigns -9 before branching. */
    ret = -9;
    if (result < 0) {
    ret = result;
    }
    }
done:
    return ret;
}

int MemCard_WriteByte(CardObj *, int);
void MemCard_WaitStatusBit2(void);
int LIBPAD_PADIF_text_26C(CardObj *inPort) {
    register CardObj *port = inPort;
    register int mode;
    register int index;
    register int offset;
    register CardObj *other asm("$19");
    register int three;
    register int result;
    register CardObj *arg;
    g_PadPortActuatorMapFn(inPort);
    mode = 0;
    if (g_MemCardCommandByte) {
        register int id = port->response_3c[0];
        {
            register int eight = 8;
            if (id >> 4 == eight) {
                register int command = port->command;
                mode = command == 0;
            }
        }
    }
    if (mode) {
        index = -1;
        offset = -240;
        while (1) {
            if (--g_MemCardWriteBlockCount <= 0)
                break;
        /* Keep the incremented byte cursor: typed indexing changes scheduling. */
        if (index >= 0)
        {
            CardObjAddress at;
            at.obj = port->subPorts;
            at.word += offset;
            g_PadPortActuatorMapFn(at.obj);
        }
        result = MemCard_WriteByte(port, (u8)g_MemCardObjReadPayloadByteFn(port, 1));
        if (result < 0)
            return result;
        setRC2wait(60);
        if (!MemCard_WaitReadyForTransfer())
            goto timeout;
        index++;
        offset += 240;
        if (index < 4)
                continue;
            break;
        }
    }
setup:
    {
        register int original = g_PadActivePortIndex;
        register int opposite;
        opposite = !original;
        if (g_MemCardWriteBlockCount >= 2) {
            register int *count;
            register CardObjAddress table;
            register int scaled;
            table.count = g_MemCardChannelCounts;
            scaled = opposite * 4;
            table.word = scaled + table.word;
            count = table.count;
            {
                register int product = opposite * 15;
                offset = product * 16;
            }
            three = 3;
            do {
                int remaining = *count;
                if (remaining < 0)
                    break;
                if (remaining > 0) {
                    /* Preserve the retail multiply and interior byte cursor. */
                    register int scaled;
                    register CardObjAddress child asm("$3");
                    scaled = remaining * sizeof(CardObj);
                    child.obj = g_MemCardObjArray;
                    child.word = offset + child.word;
                    child.obj = child.obj->subPorts;
                    child.word += scaled;
                    other = child.obj - 1;
                    g_MemCardResponseHandler(other);
                }
                {
                    register int value = *count;
                    if (value != three) {
                    if (value < 4) {
                        arg = port;
                        if (value >= 2)
                            goto call_transfer;
                        if (value < 0)
                            goto call_transfer;
                        goto primary;
                    }
                    arg = port;
                    if (value != 4)
                        goto call_transfer;
                    *count = three;
                    }
                    else
                    {
                paired:
                    do {
                    g_MemCardResponseHandler(other - 1);
                    result = 1;
                    break;
                primary:
                    {
                        register CardObjAddress base asm("$2");
                        base.obj = g_MemCardObjArray;
                        other = (CardObj *)(base.bytes + offset);
                    }
                    g_MemCardResponseHandler(other);
                    g_CardSwapByteFieldFn(other);
                    result = -1;
                    } while (0);
                    *count = result;
                }
            transfer:
                arg = port;
                    }
            call_transfer:
                result = _padSioRW(port, (u8)g_MemCardObjReadPayloadByteFn(arg, mode));
                if (result < 0)
                    return result;
                setRC2wait(60);
                if (!MemCard_WaitReadyForTransfer()) {
                timeout:
                    result = -3;
                    goto done;
                }
                --g_MemCardWriteBlockCount;
            } while (g_MemCardWriteBlockCount >= 2);
        }
    }
    while (--g_MemCardWriteBlockCount > 0) {
        result = _padSioRW(port, (u8)g_MemCardObjReadPayloadByteFn(port, mode));
        if (result < 0)
            return result;
        setRC2wait(60);
        {
            int ready = MemCard_WaitReadyForTransfer();
            if (!ready) {
                result = -3;
                return result;
            }
        }
    }
    MemCard_WaitStatusBit2();
    port->response_3c[port->response_index++] = *g_PadSioDataPtr;
    g_PadDmaCompleteFn(0);
    result = 0;
done:
    return result;
}
