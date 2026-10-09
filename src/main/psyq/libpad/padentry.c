/* ASSEMBLER: GNU */
/* Psy-Q LIBPAD PADENTRY.OBJ: PadChkVsync, PadStartCom, PadStopCom, PadChkMtap, PadGetState, PadInfoMode, PadInfoAct, PadInfoComb, PadSetActAlign, PadSetMainMode, PadSetAct. */
#include "pe1/card_obj.h"

int _padChkVsync(void);

extern void _padStartCom(void);

void _padStopCom(void);

/* The duplicated `return 1` bodies for cases 3 and 2 are load-bearing: a
 * shared case body makes GCC merge {2,3} into one range compare, while
 * retail's case tree tests ==3, <4, ==2, ==6 separately. */

void PadChkVsync(void) {
    _padChkVsync();
}

void PadStartCom(void) {
    _padStartCom();
}

void PadStopCom(void) {
    _padStopCom();
}

int PadChkMtap(int arg0)
{
  register int index;
  int offset;
  int addr;
  if (g_MemCardCommandByte != 0)
  {
    index = arg0 >> 4;
    offset = ((index << 4) - index) << 4;
    addr = offset + ((int) g_MemCardObjArray);
    return (*((unsigned char *) (addr + 0xE8))) == 8;
  }
  return 0;
}

int PadGetState(int channel) {
    int obj;

    obj = (int)g_MemCardObjLookupFn(channel);
    if ((*(unsigned int *)(obj + 0x34) & 0xFFFF0000) == 0
        && (obj == *(int *)(obj + 0x10) || *(unsigned char *)(obj + 0x38) == 0)
        && **(unsigned char **)(obj + 0x30) == 0) {
        return *(unsigned char *)(obj + 0x49);
    }

    switch (*(unsigned char *)(obj + 0x49)) {
    case 3:
        return 1;
    case 2:
        return 1;
    case 6:
        return 4;
    default:
        return *(unsigned char *)(obj + 0x49);
    }
}

int PadInfoMode(int channel, int mode, int index) {
    CardObj *obj;

    obj = g_MemCardObjLookupFn(channel);
    switch (mode) {
    case 1:
        return obj->field_e8;
    case 2:
        return obj->field_e6;
    case 3:
        return obj->field_e4;
    case 4:
        if (index < 0) {
            return obj->field_e3;
        }
        if (index < obj->field_e3) {
            return obj->modeTable[index];
        }
        goto late_fail;
    case 100:
        return obj->field_4c;
    default:
        return 0;
    }

late_fail:
    return 0;
}

int PadInfoAct(int channel, int index, int field) {
    CardObj *obj;
    PadCapabilityRecord *entry;

    obj = g_MemCardObjLookupFn(channel);
    if (index < 0) {
        return obj->field_e9;
    }
    if (index >= obj->field_e9) {
        return 0;
    }

    entry = &obj->capabilities[index];
    switch (field) {
    case 1:
        return entry->protocol[0];
    case 2:
        return entry->protocol[1];
    case 3:
        return entry->payloadBytes;
    case 4:
        return entry->activationCost;
    case 5:
        return entry->high_bit;
    default:
        return 0;
    }
}

int PadInfoComb(int channel, int index0, int index1) {
    CardObj *obj;
    PadCombinationRecord *entry;

    obj = g_MemCardObjLookupFn(channel);
    if (index0 < 0) {
        return obj->field_ea;
    }
    if (index0 >= obj->field_ea) {
        return 0;
    }
    entry = &obj->combinations[index0];
    if (index1 < 0) {
        return entry->length;
    }
    if (index1 >= entry->length) {
        return 0;
    }
    return entry->data[index1];
}
int _padSetActAlign(CardObj *obj, int command);

void PadSetActAlign(int channel, int command) {
    _padSetActAlign(g_MemCardObjLookupFn(channel), command);
}
void _padSetMainMode(CardObj *obj, int byte1, int byte2);

void PadSetMainMode(int channel, unsigned char byte1, unsigned char byte2) {
    _padSetMainMode(g_MemCardObjLookupFn(channel), byte1, byte2);
}
void _padSetAct(CardObj *obj, int payload, int size);

void PadSetAct(int channel, int payload, int size) {
    _padSetAct(g_MemCardObjLookupFn(channel), payload, size);
}

unsigned int gap_memcard_card_obj_tail_731BC[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
