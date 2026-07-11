/* MASPSX_FLAGS: --stack-return-delay */

#include "include_asm.h"

#include "include_asm.h"

#include "pe1/card_obj.h"

typedef unsigned char u8;

extern unsigned char g_MemCardObjStorage[];

void CardObj_SwapByteField(CardObj *ptr) {
    unsigned char value = ptr->command;
    ptr->command = 0;
    ptr->saved_command = value;
}

int CardObj_ReadPayloadByte(CardObj *arg0) {
    int index;
    int mode;
    u8 *ptr;

    index = arg0->payload_index - 3;
    mode = arg0->command;
    if (mode == 0) {
        goto mode0;
    }
    if (mode == 0x4D) {
        goto mode4D;
    }
    goto other;

mode0:
    if (index < 6) {
        if (arg0->field_57[index] == 0) {
            return 0;
        }
    }

    if (index >= arg0->payload_28_len) {
        return 0;
    }
    ptr = arg0->payload_28 + index;

load:
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    return *ptr;

mode4D:
    if (index < arg0->payload_2c_len) {
        ptr = arg0->payload_2c + index;
        goto load;
    }
    return 0xFF;

other:
    if (index >= arg0->payload_2c_len) {
        return 0;
    }
    return arg0->payload_2c[index];
}

extern int D_8009B76C;

void bzero(void *dst, int len);

void Render_AccumParticleAlpha(CardObj *obj) {
    int count;
    int i;
    int j;
    int offset;
    int found;
    int mask;
    int next;
    unsigned char *ptr;
    unsigned char *tags;

    bzero(obj->field_57, 6);

    if (obj->field_e6 != 0 && obj->payload_28 != 0) {
        count = 6;
        if (obj->payload_28_len < 7) {
            count = obj->payload_28_len;
        }

        if (obj->field_e9 != 0) {
            offset = 0;
            for (i = 0; i < obj->field_e9; i++) {
                found = 0;
                ptr = obj->field_04 + offset;
                if (ptr[2] != 0) {
                    mask = 0xFF;
                } else {
                    mask = 1;
                }

                tags = (unsigned char *)obj + 0x5D;
                ptr = obj->payload_28;
                for (j = 0; j < count; j++) {
                    if (*tags == i && (*ptr & mask) != 0) {
                        found = 1;
                        break;
                    }
                    tags++;
                    ptr++;
                }

                if (found != 0) {
                    next = D_8009B76C + obj->field_04[offset + 3];
                    if (next < 0x3D) {
                        D_8009B76C = next;
                    } else {
                        found = 0;
                    }
                }

                if (found != 0) {
                    tags = (unsigned char *)obj + 0x5D;
                    ptr = obj->field_57;
                    for (j = 0; j < count; j++) {
                        if (*tags == i) {
                            *ptr = 1;
                        }
                        tags++;
                        ptr++;
                    }
                }

                offset += 5;
            }
        }
    } else {
        if (((unsigned int)(obj->field_e8 - 4) < 2) || obj->field_e8 == 7) {
            if (obj->field_e6 == 0 && obj->payload_28_len >= 2) {
                ptr = obj->payload_28;
                if ((ptr[0] & 0xC0) == 0x40 && (ptr[1] & 1) != 0) {
                    if (D_8009B76C + 0xA < 0x3D) {
                        obj->field_57[1] = 1;
                        obj->field_57[0] = 1;
                        D_8009B76C += 0xA;
                        return;
                    }
                }
            }
        }

        if (obj->field_e8 == 3) {
            obj->field_57[0] = 1;
        } else if (obj->field_e6 == 0) {
            for (i = 5; i >= 0; i--) {
                obj->field_57[i] = 1;
            }
        }
    }
}

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
