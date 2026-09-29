#ifndef PE1_PSYQ_CALLBACKS_H
#define PE1_PSYQ_CALLBACKS_H

#include "pe1/psyq_types.h"

typedef void (*PsyqVoidCallback)(void);
typedef PsyqVoidCallback PsyqInterruptHandler;
typedef PsyqVoidCallback DsCallback;
typedef void (*PsyqEventCallback)(u_char event, u_char *result);
typedef PsyqEventCallback DsEventCallback;

/* Public signatures from Psy-Q LIBETC.H. */
int ResetCallback(void);
int StopCallback(void);
int RestartCallback(void);
int CheckCallback(void);
int VSyncCallback(PsyqInterruptHandler callback);

PsyqInterruptHandler InterruptCallback(int channel, PsyqInterruptHandler callback);
PsyqInterruptHandler DMACallback(int channel, PsyqInterruptHandler callback);

#endif
