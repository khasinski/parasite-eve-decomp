#include "pe1/psyq_ds.h"

extern void (*g_DsStartCallback)(int);

void LIBDS_DSSYS_2_text_13CC(int arg0) {
    if (g_DsStartCallback != 0) {
        g_DsStartCallback((unsigned char)arg0);
    }
}
