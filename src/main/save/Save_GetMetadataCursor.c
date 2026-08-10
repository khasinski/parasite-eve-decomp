/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/save.h"

unsigned char *Save_GetMetadataCursor(void) {
    return g_SaveMetadataCursor;
}
