#include "common.h"

#include "pe1/card_obj.h"

extern u8 D_800A5B70[];
extern int D_8009B764;
extern int D_8009B778;
extern int D_8009B768;
extern int D_8009B77C[];
extern u8 *D_8009B7BC;

void MemCard_OutputHandler(void *obj);
void CardObj_SwapByteField(CardObj *ptr);
int MemCard_DmaProcess(void *obj);

void MemCard_DmaCompleteCallback(int status) {
    register int value asm("$5");
    u8 *base;
    int minus_nine;
    int *status_slots;
    u8 *obj;
    register int zero asm("$0");
    u8 *dma;
    int index;
    int cur_offset;
    int next_offset;
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
