/* MASPSX_FLAGS: --stack-return-delay */

#include "include_asm.h"

#include "include_asm.h"

typedef unsigned char u8;

typedef struct {
    char pad00[0x28];
    u8 *field28;
    u8 *field2C;
    char pad30[4];
    u8 field34;
    u8 field35;
    u8 field36;
    char pad37[0xE];
    u8 field45;
    char pad46[0x11];
    u8 field57[6];
} Unk800847B0;

extern unsigned char g_MemCardObjStorage[];
extern u8 D_800A5B70[];
extern int D_8009B764;
extern int D_8009B778;
extern int D_8009B768;
extern int D_8009B77C[];
extern u8 *D_8009B7BC;

void MemCard_OutputHandler(void *obj);
void CardObj_SwapByteField(unsigned char *ptr);
int MemCard_DmaProcess(void *obj);

/* The inner do { } while (0) is a load-bearing scheduler fence: it keeps the
 * candidate advance out of the value/index update order so the bnez delay
 * slot gets the pointer increment, matching retail. */

void CardObj_ResetFields(char *arg0) {
    int count;
    int value;
    char *ptr;

    if (*(unsigned char *)(arg0 + 0x49) != 0) {
        ptr = arg0 + 0x5D;
        value = 0xFF;
        count = 5;
        *(char *)(arg0 + 0x49) = 0;
        *(char *)(arg0 + 0x46) = 0;
        *(short *)(arg0 + 0xE6) = 0;
        *(int *)(arg0 + 0x14) = 0;
        *(int *)(arg0 + 0x18) = 0;
        *(char *)(arg0 + 0xE3) = 0;
        *(char *)(arg0 + 0xE4) = 0;
        *(short *)(arg0 + 0xE6) = 0;
        *(char *)(arg0 + 0xE9) = 0;
        *(char *)(arg0 + 0xEA) = 0;
        *(int *)(arg0 + 0x0) = 0;
        *(int *)(arg0 + 0x4) = 0;
        *(int *)(arg0 + 0x8) = 0;

        do {
            *ptr = value;
            count -= 1;
            ptr += 1;
        } while (count >= 0);
    }
}

void MemCard_DmaCompleteCallback(int status) {
    register int value asm("$5");
    register u8 *base asm("$17");
    register int minus_nine asm("$19");
    register int *status_slots asm("$18");
    register u8 *obj asm("$16");
    register int zero asm("$0");
    u8 *dma;
    int index;
    register int cur_offset asm("$2");
    register int next_offset asm("$4");
    int done;

    value = status;
    base = D_800A5B70;
    minus_nine = -9;
    status_slots = D_8009B77C;

loop:
    index = D_8009B764;
    cur_offset = index * 0xF0;
    obj = (u8 *)(cur_offset + (int)base);
    if (value != minus_nine) {
        if (value == 0) {
            status_slots[index] = zero;
        } else {
            MemCard_OutputHandler(obj);
            CardObj_SwapByteField(obj);
        }
    }

    dma = D_8009B7BC;
    asm volatile("" : "=r"(dma) : "0"(dma));
    index = D_8009B764;
    asm volatile("" : "=r"(index) : "0"(index));
    D_8009B768 = 0;
    *(short *)(dma + 0xA) = 0;
    index += 1;
    D_8009B764 = index;
    if (index <= D_8009B778) {
        next_offset = index * 0xF0;
        done = MemCard_DmaProcess((u8 *)(next_offset + (int)base));
    } else {
        done = 1;
    }
    value = 0xFFFF;
    asm volatile("" : "=r"(value) : "0"(value));
    if (done == 0) {
        goto loop;
    }
}

void CardObj_SwapByteField(unsigned char *ptr) {
    unsigned char value = ptr[0x36];
    ptr[0x36] = 0;
    ptr[0x37] = value;
}

int CardObj_ReadPayloadByte(Unk800847B0 *arg0) {
    int index;
    int mode;
    u8 *ptr;

    index = arg0->field45 - 3;
    mode = arg0->field36;
    if (mode == 0) {
        goto mode0;
    }
    if (mode == 0x4D) {
        goto mode4D;
    }
    goto other;

mode0:
    if (index < 6) {
        if (arg0->field57[index] == 0) {
            return 0;
        }
    }

    if (index >= arg0->field34) {
        return 0;
    }
    ptr = arg0->field28 + index;

load:
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    return *ptr;

mode4D:
    if (index < arg0->field35) {
        ptr = arg0->field2C + index;
        goto load;
    }
    return 0xFF;

other:
    if (index >= arg0->field35) {
        return 0;
    }
    return arg0->field2C[index];
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/card_obj6", Render_AccumParticleAlpha);

int CardObj_GetChannelId(unsigned char *entry)
{
  unsigned char *candidate;
  int index;
  int value;
  index = 0;
  value = 0x10;
  candidate = g_MemCardObjStorage;
  for (; index < 2; index++)
  {
    if (entry == candidate)
    {
      return value;
    }
    value += 0x10;
 do { candidate += 0xF0; } while (0);
  }

  return 0xFF;
}
