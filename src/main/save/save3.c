/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/save.h"

void Save_PostInitStub(void) {
}

int Save_GetMetadataWindowIndex(void) {
    return g_SaveMetadataWindowIndex;
}
