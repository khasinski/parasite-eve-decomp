/* Psy-Q LIBDS DSREAD.OBJ: DS_read_cbsync. */
#include "pe1/psyq_ds.h"

void DS_read_cbsync(unsigned char arg0) {
    if (arg0 == 2) {
        DsStartReadySystem(DS_read_cbready, -1);
    }
}
