#ifndef PSYQ_LIBPAD_PADIF_EXTERNS_H
#define PSYQ_LIBPAD_PADIF_EXTERNS_H

#include "common.h"
#include "pe1/card_obj.h"

extern int g_MemCardDispatchResult;
extern void (*D_8009B750)(void);
extern void (*D_8009B754)(void);
extern int D_8009B760;
extern int D_8009B228;
extern void (*D_8009B21C)(void);
extern void (*D_8009B218)(void);
extern int g_PadActivePortIndex;
extern int g_MemCardPort1Present;
extern int g_MemCardWriteBlockCount;
extern void (*g_PadPortActuatorMapFn)(CardObj *);
extern void (*g_CardSwapByteFieldFn)(CardObj *);
extern int g_MemCardCommandByte, g_PadActivePortIndex, g_MemCardWriteBlockCount, g_MemCardChannelCounts[];
extern volatile u8 *g_PadSioDataPtr;

#endif /* PSYQ_LIBPAD_PADIF_EXTERNS_H */
