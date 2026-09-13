/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/card_obj.h"

typedef struct MemCardTransferControl {
    u8 reserved00[0xA];
    u16 status;
} MemCardTransferControl;

extern CardObj D_800A5B70[];
extern int D_8009B764;
/* Separate views prevent GCC from hoisting this IRQ-shared index. */
extern int g_MemCardDmaNextIndex asm("D_8009B764");
extern int g_MemCardDmaStoredIndex asm("D_8009B764");
extern int D_8009B768;
extern int g_MemCardPort2Present;
extern int D_8009B77C[];
extern MemCardTransferControl *D_8009B7BC;

void _dirFailAuto(CardObj *obj);
void CardObj_SwapByteField(CardObj *obj);
int MemCard_DmaProcess(CardObj *obj);

int MemCard_DmaCompleteCallback(int result) {
    register int callbackResult asm("$5") = result;
    CardObj *objects;
    int ignoredResult;
    int *channelResults;
    CardObj *obj;
    int index;
    register MemCardTransferControl *control asm("$2");
    register int limit asm("$2");
    int offset;
    int nextResult;

    asm("" : "=r"(objects) : "0"(&D_800A5B70[0]));
    ignoredResult = -9;
    asm("" : "=r"(channelResults) : "0"(&D_8009B77C[0]));

    do {
        index = D_8009B764;
        offset = ((index << 4) - index) << 4;
        obj = (CardObj *)(offset + (int)objects);

        if (callbackResult != ignoredResult) {
            if (callbackResult == 0) {
                asm("" : "+r"(callbackResult));
                offset = index << 2;
                *(int *)(offset + (int)channelResults) = 0;
            } else {
                _dirFailAuto(obj);
                CardObj_SwapByteField(obj);
            }
        }

        asm("" : "=r"(control) : "0"(D_8009B7BC));
        index = g_MemCardDmaNextIndex;
        asm("" : "+r"(index));
        D_8009B768 = 0;
        asm volatile("" : : : "memory");
        control->status = 0;
        asm("" : "=r"(limit) : "0"(g_MemCardPort2Present));
        index += 1;
        g_MemCardDmaStoredIndex = index;
        asm volatile("" : : : "memory");
        if (limit >= index) {
            nextResult = MemCard_DmaProcess(
                (CardObj *)((((index << 4) - index) << 4) +
                            (int)objects));
        } else {
            nextResult = 1;
        }
        callbackResult = 0xFFFF;
    } while (nextResult == 0);

    return nextResult;
}
