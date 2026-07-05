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

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/card_obj6", MemCard_DmaCompleteCallback);

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
