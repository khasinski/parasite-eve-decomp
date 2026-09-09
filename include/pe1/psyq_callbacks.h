#ifndef PE1_PSYQ_CALLBACKS_H
#define PE1_PSYQ_CALLBACKS_H

typedef void (*PsyqInterruptHandler)(void);

/* Public signatures from Psy-Q LIBETC.H. */
int ResetCallback(void);
int StopCallback(void);
int RestartCallback(void);
int CheckCallback(void);
int VSyncCallback(PsyqInterruptHandler callback);

#endif
