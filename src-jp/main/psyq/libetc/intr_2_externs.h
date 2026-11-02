#ifndef PSYQ_LIBETC_INTR_2_EXTERNS_H
#define PSYQ_LIBETC_INTR_2_EXTERNS_H

#include "common.h"

extern u16 g_IntrState[];
extern u16 *g_IntrStatusRegPtr;
extern volatile u16 *D_80095674;
extern volatile u16 *D_8009513C;
extern u32 *g_IntrDmaControlRegPtr;

#endif /* PSYQ_LIBETC_INTR_2_EXTERNS_H */
