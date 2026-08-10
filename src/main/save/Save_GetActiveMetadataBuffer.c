/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/save.h"

char *Save_GetActiveMetadataBuffer(void) {
    char *buffer;

    buffer = (char *)g_SaveRuntimeMetadataWindows;

    if (g_SaveMetadataWindowIndex != 0) {
        buffer += 0x10;
    }

    return buffer;
}
