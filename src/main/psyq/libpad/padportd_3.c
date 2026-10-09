/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBPAD PADPORTD, part 3 of 4: MemCard_DmaCompleteCallback. */

#include "common.h"
#include "pe1/card_obj.h"
#include "pe1/memcard_state.h"

extern CardObj D_800A5B70[];
extern int D_8009B764;
/* Separate views prevent GCC from hoisting this IRQ-shared index. */
extern int g_MemCardDmaNextIndex asm("D_8009B764");
extern int g_MemCardDmaStoredIndex asm("D_8009B764");
extern int D_8009B768;
extern int g_MemCardPort2Present;
extern int D_8009B77C[];
extern MemCardSioRegisters *D_8009B7BC;

void _dirFailAuto(CardObj *obj);
int _padInitSioMode(CardObj *obj);

int MemCard_DmaCompleteCallback(int result) {
    register int callbackResult asm("$5") = result;
    CardObj *objects;
    int ignoredResult;
    int *channelResults;
    CardObj *obj;
    int index;
    MemCardSioRegisters *control;
    int limit;
    int offset;
    int nextResult;

    objects = &D_800A5B70[0];
    ignoredResult = -9;
    channelResults = &D_8009B77C[0];

    do {
        index = D_8009B764;
        offset = index * sizeof(CardObj);
        obj = (CardObj *)(offset + (int)objects);

        if (callbackResult != ignoredResult) {
            if (callbackResult == 0) {
                asm("" : "+r"(callbackResult));
                channelResults[index] = 0;
            } else {
                _dirFailAuto(obj);
                CardObj_SwapByteField(obj);
            }
        }

        index = g_MemCardDmaNextIndex;
        asm("" : "=r"(control), "+r"(index) : "0"(D_8009B7BC));
        D_8009B768 = 0;
        asm volatile("" : : : "memory");
        control->control = 0;
        limit = g_MemCardPort2Present;
        index += 1;
        g_MemCardDmaStoredIndex = index;
        asm volatile("" : : : "memory");
        if (limit >= index) {
            nextResult = _padInitSioMode(
                (CardObj *)((((index << 4) - index) << 4) +
                            (int)objects));
        } else {
            nextResult = 1;
        }
        callbackResult = 0xFFFF;
    } while (nextResult == 0);

    return nextResult;
}
