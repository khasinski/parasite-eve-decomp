#include "pe1/psyq_ds.h"

extern void (*g_LibDsReadyCallback)(int);

void LIBDS_DSSYS_2_text_13CC(int arg0) {
    if (g_LibDsReadyCallback != 0) {
        g_LibDsReadyCallback((unsigned char)arg0);
    }
}
