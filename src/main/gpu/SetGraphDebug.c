/* GCC_VERSION: 2.8.1 */

#include "common.h"
#include "pe1/gpu_state.h"

extern char D_80011814[];

int SetGraphDebug(int debugLevel) {
    register u8 *currentDebugLevel asm("$3");
    register int result asm("$2");
    void (*debugPrint)(char *message, int debugLevel, int queueState,
                       int drawState);
    int currentLevel;
    int queueState;
    int drawState;
    int oldDebugLevel;

    currentDebugLevel = &D_8009574C.queueState.debugLevel;
    /* Preserve the shared base used for the adjacent GPU state bytes. */
    asm("" : "+r"(currentDebugLevel));
    oldDebugLevel = *currentDebugLevel;
    *currentDebugLevel = debugLevel;
    result = oldDebugLevel;

    if ((u8)debugLevel == 0) {
        return result;
    }

    debugPrint = D_80095748;
    /* Keep the callback load ahead of its arguments. */
    asm volatile("" : "+r"(debugPrint));
    currentLevel = currentDebugLevel[0];
    queueState = currentDebugLevel[-2];
    drawState = currentDebugLevel[1];
    /* Materialize the byte arguments before the format string address. */
    asm volatile("" : "+r"(currentLevel), "+r"(queueState), "+r"(drawState));
    debugPrint(D_80011814, currentLevel, queueState, drawState);
    result = oldDebugLevel;
    return result;
}
